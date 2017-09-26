#pragma once

#ifndef OOSL_LITERAL_AST_H
#define OOSL_LITERAL_AST_H

#include <climits>

#include "../node/inplace_node.h"

#include "ast.h"

#define OOSL_DEFINE_NUMERIC_STRUCT(name, type)	\
struct OOSL_AST_NAME(name){						\
	type value;									\
};

#define OOSL_DEFINE_STRING_STRUCT(name)			\
struct OOSL_AST_NAME(name){						\
	boost::optional<string_prefix_type> pref;	\
	std::string value;							\
};

#define OOSL_DEFINE_STRING_SEQUENCE_STRUCT(name)\
struct OOSL_AST_NAME(OOSL_AST_JOIN(name, _sequence)){\
	std::vector<OOSL_AST_NAME(name)> value;		\
};

#define OOSL_ADAPT_NUMERIC_STRUCT(name, type)	\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_QNAME(name),						\
	(type, value)								\
)

#define OOSL_ADAPT_STRING_STRUCT(name)			\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_QNAME(name),						\
	(boost::optional<oosl::lexer::string_prefix_type>, pref)\
	(std::string, value)						\
)

#define OOSL_ADAPT_STRING_SEQUENCE_STRUCT(name)	\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_QNAME(OOSL_AST_JOIN(name, _sequence)),\
	(std::vector<OOSL_AST_QNAME(name)>, value)	\
)

namespace oosl{
	namespace lexer{
		enum class numeric_suffix_type{
			nil,
			i8,
			ui8,
			i16,
			ui16,
			i32,
			ui32,
			i64,
			ui64,
			i128,
			ui128,
			f32,
			f64,
			f128,
		};

		enum class string_prefix_type{
			nil,
			wide,
		};

		OOSL_DEFINE_NUMERIC_STRUCT(integer, long long);
		OOSL_DEFINE_NUMERIC_STRUCT(real, long double);

		struct OOSL_AST_NAME(rad){
			int base;
			std::string value;
		};

		struct OOSL_AST_NAME(num){
			typedef boost::variant<
				OOSL_AST_NAME(rad),
				OOSL_AST_NAME(real),
				OOSL_AST_NAME(integer)
			> num_type;
			num_type num;
			boost::optional<numeric_suffix_type> suff;
		};

		struct OOSL_AST_JOIN(OOSL_AST_NAME(num), _visitor) : public boost::static_visitor<oosl::node::object::ptr_type>{
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<float> f32_inplace_type;
			typedef oosl::node::inplace<double> f64_inplace_type;
			typedef oosl::node::inplace<long double> f128_inplace_type;

			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			OOSL_AST_JOIN(OOSL_AST_NAME(num), _visitor)(OOSL_AST_NAME(num) &object)
				: object_(&object){}

			node_ptr_type operator()(const OOSL_AST_NAME(rad) &value){
				std::size_t index = 0;
				long long converted_value;

				try{
					converted_value = std::stoll(value.value, &index, value.base);
					if (index < value.value.size())
						throw oosl::common::error_codes::bad_integer;
				}
				catch (...){
					throw oosl::common::error_codes::bad_integer;
				}

				return this->operator()(OOSL_AST_NAME(integer){ converted_value });
			}

			node_ptr_type operator()(const OOSL_AST_NAME(integer) &value){
				switch (object_->suff.value_or(numeric_suffix_type::nil)){
				case numeric_suffix_type::nil://No suffix
				case numeric_suffix_type::i32:
					break;
				case numeric_suffix_type::i8:
					return make_node<oosl::node::inplace<__int8>>(value.value, node_index_type{}, "i8");
				case numeric_suffix_type::ui8:
					return make_node<oosl::node::inplace<unsigned __int8>>(value.value, node_index_type{}, "ui8");
				case numeric_suffix_type::i16:
					return make_node<oosl::node::inplace<__int16>>(value.value, node_index_type{}, "i16");
				case numeric_suffix_type::ui16:
					return make_node<oosl::node::inplace<unsigned __int16>>(value.value, node_index_type{}, "ui16");
				case numeric_suffix_type::ui32:
					return make_node<oosl::node::inplace<unsigned __int32>>(value.value, node_index_type{}, "ui32");
				case numeric_suffix_type::i64:
					return make_node<oosl::node::inplace<__int64>>(value.value, node_index_type{}, "i64");
				case numeric_suffix_type::ui64:
					return make_node<oosl::node::inplace<unsigned __int64>>(value.value, node_index_type{}, "ui64");
				default:
					throw oosl::common::error_codes::invalid_suffix;
				}

				return make_node<oosl::node::inplace<__int32>>(value.value, node_index_type{}, "i32");
			}

			node_ptr_type operator()(const OOSL_AST_NAME(real) &value){
				switch (object_->suff.value_or(numeric_suffix_type::nil)){
				case numeric_suffix_type::nil://No suffix
				case numeric_suffix_type::f64://double
					break;
				case numeric_suffix_type::f32://float
					return make_node<oosl::node::inplace<float>>(value.value, node_index_type{}, "f32");
				case numeric_suffix_type::f128://long double
					return make_node<oosl::node::inplace<long double>>(value.value, node_index_type{}, "f128");
				default:
					throw oosl::common::error_codes::invalid_suffix;
				}

				return make_node<oosl::node::inplace<double>>(value.value, node_index_type{}, "f64");
			}

			template <typename node_type, typename value_type>
			node_ptr_type make_node(value_type value, const node_index_type &index, const std::string &suffix){
				typedef typename node_type::value_type target_value_type;
				if (sizeof(value_type) > sizeof(target_value_type) && value > static_cast<value_type>(std::numeric_limits<target_value_type>::max()))
					throw oosl::common::error_codes::number_too_big;

				return std::make_shared<node_type>(node_id_type::literal_, node_index_type{}, [=](inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(static_cast<target_value_type>(value));
						return true;
					case inplace_target_type::print:
						if (std::is_floating_point_v<target_value_type>)
							*reinterpret_cast<std::string *>(out) = (oosl::common::controller::real_to_string<std::string>(static_cast<target_value_type>(value)) + suffix);
						else//Integral value
							*reinterpret_cast<std::string *>(out) = (std::to_string(static_cast<target_value_type>(value)) + suffix);
						return true;
					default:
						break;
					}

					return false;
				}, static_cast<target_value_type>(value));
			}

			OOSL_AST_NAME(num) *object_;
		};

		OOSL_DEFINE_STRING_STRUCT(character);
		OOSL_DEFINE_STRING_STRUCT(string);
		OOSL_DEFINE_STRING_STRUCT(raw);

		OOSL_DEFINE_STRING_SEQUENCE_STRUCT(string);
		OOSL_DEFINE_STRING_SEQUENCE_STRUCT(raw);
	}
}

OOSL_ADAPT_NUMERIC_STRUCT(integer, long long);
OOSL_ADAPT_NUMERIC_STRUCT(real, long double);

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(rad),
	(int, base)
	(std::string, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(num),
	(OOSL_AST_QNAME(num)::num_type, num)
	(boost::optional<oosl::lexer::numeric_suffix_type>, suff)
)

OOSL_ADAPT_STRING_STRUCT(character);
OOSL_ADAPT_STRING_STRUCT(string);
OOSL_ADAPT_STRING_STRUCT(raw);

OOSL_ADAPT_STRING_SEQUENCE_STRUCT(string);
OOSL_ADAPT_STRING_SEQUENCE_STRUCT(raw);

#endif /* !OOSL_LITERAL_AST_H */
