#pragma once

#ifndef OOSL_LITERAL_AST_H
#define OOSL_LITERAL_AST_H

#include <climits>

#include "../node/inplace_node.h"

#include "ast.h"

#define OOSL_DEFINE_NUMERIC_STRUCT(name, type)	\
struct OOSL_AST_NAME(name){						\
	type value;									\
	boost::optional<numeric_suffix_type> suff;	\
};

#define OOSL_DEFINE_INTEGER_STRUCT(name) OOSL_DEFINE_NUMERIC_STRUCT(name, long long)
#define OOSL_DEFINE_REAL_STRUCT(name) OOSL_DEFINE_NUMERIC_STRUCT(name, long double)

#define OOSL_DEFINE_STRING_STRUCT(name)			\
struct OOSL_AST_NAME(name){						\
	boost::optional<string_prefix_type> pref;	\
	std::string value;							\
};

#define OOSL_ADAPT_NUMERIC_STRUCT(name, type)	\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_QNAME(name),						\
	(type, value)								\
	(boost::optional<oosl::lexer::numeric_suffix_type>, suff)\
)

#define OOSL_ADAPT_INTEGER_STRUCT(name) OOSL_ADAPT_NUMERIC_STRUCT(name, long long)
#define OOSL_ADAPT_REAL_STRUCT(name) OOSL_ADAPT_NUMERIC_STRUCT(name, long double)

#define OOSL_ADAPT_STRING_STRUCT(name)			\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_QNAME(name),						\
	(boost::optional<oosl::lexer::string_prefix_type>, pref)\
	(std::string, value)						\
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

		struct numeric_ast_to_node_helper{
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			template <typename node_type, typename value_type>
			static node_ptr_type make_node(value_type value, const node_index_type &index, const std::string &suffix){
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
		};

		OOSL_DEFINE_INTEGER_STRUCT(integer);

		OOSL_AST_TO_NODE(integer){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(integer) &ast){
				typedef oosl::node::index node_index_type;
				switch (ast.suff.value_or(numeric_suffix_type::nil)){
				case numeric_suffix_type::nil://No suffix
				case numeric_suffix_type::i32:
					break;
				case numeric_suffix_type::i8:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<__int8>>(ast.value, node_index_type{}, "i8");
				case numeric_suffix_type::ui8:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<unsigned __int8>>(ast.value, node_index_type{}, "ui8");
				case numeric_suffix_type::i16:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<__int16>>(ast.value, node_index_type{}, "i16");
				case numeric_suffix_type::ui16:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<unsigned __int16>>(ast.value, node_index_type{}, "ui16");
				case numeric_suffix_type::ui32:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<unsigned __int32>>(ast.value, node_index_type{}, "ui32");
				case numeric_suffix_type::i64:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<__int64>>(ast.value, node_index_type{}, "i64");
				case numeric_suffix_type::ui64:
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<unsigned __int64>>(ast.value, node_index_type{}, "ui64");
				default:
					throw oosl::common::error_codes::invalid_suffix;
				}

				return numeric_ast_to_node_helper::make_node<oosl::node::inplace<__int32>>(ast.value, node_index_type{}, "i32");
			}
		};

		OOSL_DEFINE_REAL_STRUCT(real);

		OOSL_AST_TO_NODE(real){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(real) &ast){
				typedef oosl::node::index node_index_type;
				switch (ast.suff.value_or(numeric_suffix_type::nil)){
				case numeric_suffix_type::nil://No suffix
				case numeric_suffix_type::f64://double
					break;
				case numeric_suffix_type::f32://float
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<float>>(ast.value, node_index_type{}, "f32");
				case numeric_suffix_type::f128://long double
					return numeric_ast_to_node_helper::make_node<oosl::node::inplace<long double>>(ast.value, node_index_type{}, "f128");
				default:
					throw oosl::common::error_codes::invalid_suffix;
				}

				return numeric_ast_to_node_helper::make_node<oosl::node::inplace<double>>(ast.value, node_index_type{}, "f64");
			}
		};

		struct OOSL_AST_NAME(rad){
			int base;
			std::string value;
			boost::optional<numeric_suffix_type> suff;
		};

		OOSL_AST_TO_NODE(rad){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(rad) &ast){
				if (ast.base < 2 || ast.base > 26)
					throw oosl::common::error_codes::bad_integer;

				std::size_t index = 0;
				long long converted_value;

				try{
					converted_value = std::stoll(ast.value, &index, ast.base);
					if (index < ast.value.size())
						throw oosl::common::error_codes::bad_integer;
				}
				catch (...){
					throw oosl::common::error_codes::bad_integer;
				}

				OOSL_AST_NAME(integer) integer_value{ converted_value, ast.suff };
				return OOSL_AST_TO_NODE_GET(integer, integer_value);
			}
		};

		struct OOSL_AST_NAME(num){
			typedef boost::variant<
				OOSL_AST_NAME(rad),
				OOSL_AST_NAME(real),
				OOSL_AST_NAME(integer)
			> num_type;
			num_type value;
		};

		OOSL_AST_VISITOR_3(num, rad, real, integer);

		OOSL_AST_TO_NODE(num){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(num) &ast){
				return OOSL_AST_APPLY_VISITOR(num, ast.value);
			}
		};

		OOSL_DEFINE_STRING_STRUCT(character);

		OOSL_AST_TO_NODE(character){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<std::string> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(character) &ast){
				auto is_wide = (ast.pref.value_or(string_prefix_type::nil) == string_prefix_type::wide);
				return std::make_shared<inplace_type>(node_id_type::literal_, node_index_type{}, [is_wide](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						return true;
					case inplace_target_type::print:
						if (is_wide)
							*reinterpret_cast<std::string *>(out) = ("L\'" + owner.value() + "\'");
						else//Non-wide
							*reinterpret_cast<std::string *>(out) = ("\'" + owner.value() + "\'");
						return true;
					default:
						break;
					}

					return false;
				}, ast.value);
			}
		};

		OOSL_DEFINE_STRING_STRUCT(string);

		OOSL_AST_TO_NODE(string){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<std::string> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(string) &ast){
				auto is_wide = (ast.pref.value_or(string_prefix_type::nil) == string_prefix_type::wide);
				return std::make_shared<inplace_type>(node_id_type::literal_, node_index_type{}, [is_wide](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						if (is_wide)
							*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(oosl::common::controller::to_wstring(owner.value()));
						else//Non-wide
							*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(owner.value());
						return true;
					case inplace_target_type::print:
						if (is_wide)
							*reinterpret_cast<std::string *>(out) = ("L\"" + owner.value() + "\"");
						else//Non-wide
							*reinterpret_cast<std::string *>(out) = ("\"" + owner.value() + "\"");
						return true;
					default:
						break;
					}

					return false;
				}, ast.value);
			}
		};

		OOSL_DEFINE_STRING_STRUCT(raw);

		OOSL_AST_TO_NODE(raw){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<std::string> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(raw) &ast){
				return nullptr;
			}
		};

		struct OOSL_AST_NAME(quote){
			typedef boost::variant<
				OOSL_AST_NAME(character),
				OOSL_AST_NAME(string),
				OOSL_AST_NAME(raw)
			> quote_type;
			quote_type value;
		};

		OOSL_AST_VISITOR_3(quote, character, string, raw);

		OOSL_AST_TO_NODE(quote){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(quote) &ast){
				return OOSL_AST_APPLY_VISITOR(quote, ast.value);
			}
		};

		struct OOSL_AST_NAME(lit){
			typedef boost::variant<
				OOSL_AST_NAME(num),
				OOSL_AST_NAME(quote)
			> lit_type;
			lit_type value;
		};

		OOSL_AST_VISITOR_2(lit, num, quote);

		OOSL_AST_TO_NODE(lit){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(lit) &ast){
				return OOSL_AST_APPLY_VISITOR(lit, ast.value);
			}
		};
	}
}

OOSL_ADAPT_INTEGER_STRUCT(integer);
OOSL_ADAPT_REAL_STRUCT(real);

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(rad),
	(int, base)
	(std::string, value)
	(boost::optional<oosl::lexer::numeric_suffix_type>, suff)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(num),
	(OOSL_AST_QNAME(num)::num_type, value)
)

OOSL_ADAPT_STRING_STRUCT(character);
OOSL_ADAPT_STRING_STRUCT(string);
OOSL_ADAPT_STRING_STRUCT(raw);

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(quote),
	(OOSL_AST_QNAME(quote)::quote_type, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(lit),
	(OOSL_AST_QNAME(lit)::lit_type, value)
)

#endif /* !OOSL_LITERAL_AST_H */
