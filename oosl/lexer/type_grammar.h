#pragma once

#ifndef OOSL_TYPE_GRAMMAR_H
#define OOSL_TYPE_GRAMMAR_H

#include "general_grammar.h"

#include "../type/primitive_type.h"
#include "../type/modified_type.h"
#include "../type/pointer_type.h"
#include "../type/array_type.h"

namespace oosl{
	namespace lexer{
		class expression_grammar;

		class storage_specifier_grammar : public typed_grammar<oosl::type::object::attribute>{
		public:
			typedef typed_grammar<oosl::type::object::attribute> base_type;
			typedef oosl::type::object::attribute type_attribute_type;

			typedef boost::spirit::qi::symbols<char, type_attribute_type> specifier_symbols_type;

			storage_specifier_grammar();

			static type_attribute_type combine(type_attribute_type left, type_attribute_type right);

			static node_ptr_type create(type_attribute_type attributes, node_ptr_type value);

			static std::string print_attributes(type_attribute_type value);

		protected:
			rule_type static_rule_;
			rule_type thread_local_rule_;
			specifier_symbols_type static_symbols_;
			specifier_symbols_type thread_local_symbols_;
		};

		struct primitive_type_helper{
			typedef oosl::type::id type_id_type;
			typedef oosl::node::object::ptr_type node_ptr_type;

			static node_ptr_type create(type_id_type value);

			static const char *other_value(type_id_type id);
		};

		class primitive_type_grammar : public grammar{
		public:
			typedef oosl::type::id type_id_type;
			typedef boost::spirit::qi::symbols<char, type_id_type> type_symbols_type;

			primitive_type_grammar();

			static node_ptr_type create(type_id_type value);

		protected:
			type_symbols_type type_symbols_;
		};

		template <oosl::type::id id>
		class other_primitive_type_grammar : public grammar{
		public:
			other_primitive_type_grammar()
				: grammar("OOSL_OTHER_PRIMITIVE_TYPE"){
				using namespace boost::spirit;

				start_ = (qi::lit(primitive_type_helper::other_value(id)))[qi::_val = create()];
			}

			static node_ptr_type create(){
				return primitive_type_helper::create(id);
			}
		};

		using auto_type_grammar = other_primitive_type_grammar<oosl::type::id::auto_>;
		using void_type_grammar = other_primitive_type_grammar<oosl::type::id::void_>;

		class decltype_grammar : public grammar{
		public:
			typedef std::shared_ptr<expression_grammar> expression_grammar_type;

			decltype_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar_type expression_;
		};

		template <bool is_array>
		class pointer_or_array_type_grammar : public grammar{
		public:
			pointer_or_array_type_grammar()
				: grammar(is_array ? "OOSL_ARRAY_TYPE" : "OOSL_POINTER_TYPE"){
				using namespace boost::spirit;

				typed_ = ('<' >> (primitive_type_ | decltype_ | identifier_compatible_) >> -(':' > qi::uint_) > '>')[qi::_val = boost::phoenix::bind(&create_typed, qi::_1, qi::_2)];

				start_ = ((is_array ? "array_t" : "pointer_t") >> -typed_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
			}

			static node_ptr_type create_typed(node_ptr_type value, boost::optional<unsigned int> dim){
				typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

				return std::make_shared<node_type>(node_id_type::type, [dim](node_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.type());
						return true;
					case inplace_target_type::print:
						reinterpret_cast<output_writer_type *>(out)->write(is_array ? "array" : "pointer_t<");
						owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));

						if (dim.is_initialized()){//Print dimension
							reinterpret_cast<output_writer_type *>(out)->write(" : ");
							reinterpret_cast<output_writer_type *>(out)->write(std::to_string(dim.value()));
						}

						reinterpret_cast<output_writer_type *>(out)->write(">");
						return true;
					case inplace_target_type::type:
						break;
					default:
						return false;
					}

					auto &value = owner.value();
					if (value.second == nullptr){//Evaluate
						value.second = value.first->type()->reflect();
						for (auto i = dim.value_or(1u); i > 0u; --i){//Create pointer|array hierarchy
							if (is_array)
								value.second = std::make_shared<oosl::type::array_type>(value.second);
							else//Pointer
								value.second = std::make_shared<oosl::type::pointer>(value.second);
						}
					}

					*reinterpret_cast<type_object_type **>(out) = value.second.get();
					return true;
				}, std::make_pair(value, nullptr));
			}

			static node_ptr_type create(boost::optional<node_ptr_type> typed){
				return (typed.is_initialized() ? typed.value() : primitive_type_helper::create(is_array ? type_id_type::array_ : type_id_type::pointer_));
			}

		protected:
			rule_type typed_;
			primitive_type_grammar primitive_type_;
			decltype_grammar decltype_;
			identifier_compatible_grammar identifier_compatible_;
		};

		using pointer_type_grammar = pointer_or_array_type_grammar<false>;
		using array_type_grammar = pointer_or_array_type_grammar<false>;

		class return_type_grammar;
		class parameter_type_grammar;
		class variadic_type_grammar;

		class function_type_grammar : public grammar{
		public:
			typedef std::shared_ptr<return_type_grammar> return_type_grammar_ptr_type;
			typedef std::shared_ptr<parameter_type_grammar> parameter_type_grammar_ptr_type;
			typedef std::shared_ptr<variadic_type_grammar> variadic_type_grammar_ptr_type;
			typedef std::vector<node_ptr_type> node_ptr_list_type;

			struct type_info{
				node_ptr_list_type params;
				boost::optional<node_ptr_type> variadic_type;
			};

			typedef boost::spirit::qi::rule<iterator_type, type_info(), skipper> custom_rule_type;

			function_type_grammar();

			static node_ptr_type create_typed(node_ptr_type return_type, const boost::optional<type_info> &info);

			static node_ptr_type create(boost::optional<node_ptr_type> typed);

		protected:
			rule_type typed_;
			custom_rule_type type_;
			return_type_grammar_ptr_type return_type_;
			parameter_type_grammar_ptr_type parameter_type_;
			variadic_type_grammar_ptr_type variadic_type_;
			void_type_grammar void_type_;
		};

		class type_grammar : public grammar{
		public:
			type_grammar();

		protected:
			primitive_type_grammar primitive_type_;
			decltype_grammar decltype_;
			pointer_type_grammar pointer_type_;
			array_type_grammar array_type_;
			function_type_grammar function_type_;
		};

		class ref_type_grammar : public grammar{
		public:
			ref_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			type_grammar type_;
			identifier_compatible_grammar identifier_compatible_;
		};

		class rval_ref_type_grammar : public grammar{
		public:
			rval_ref_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			type_grammar type_;
			identifier_compatible_grammar identifier_compatible_;
		};

		class ref_auto_type_grammar : public grammar{
		public:
			ref_auto_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			auto_type_grammar type_;
		};

		class rval_ref_auto_type_grammar : public grammar{
		public:
			rval_ref_auto_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			auto_type_grammar type_;
		};

		class return_type_grammar : public grammar{
		public:
			return_type_grammar();

		protected:
			type_grammar type_;
			ref_type_grammar ref_type_;
			auto_type_grammar auto_type_;
			ref_auto_type_grammar ref_auto_type_;
			identifier_compatible_grammar identifier_compatible_;
		};

		class parameter_type_grammar : public grammar{
		public:
			parameter_type_grammar();

		protected:
			return_type_grammar return_type_;
			rval_ref_type_grammar rval_ref_type_;
			rval_ref_auto_type_grammar rval_ref_auto_type_;
		};

		class variadic_type_grammar : public grammar{
		public:
			typedef std::shared_ptr<function_type_grammar> function_type_grammar_ptr_type;

			variadic_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			parameter_type_grammar parameter_type_;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	oosl::lexer::function_type_grammar::type_info,
	(oosl::lexer::function_type_grammar::node_ptr_list_type, params)
	(boost::optional<oosl::lexer::grammar::node_ptr_type>, variadic_type)
)

#endif /* !OOSL_TYPE_GRAMMAR_H */
