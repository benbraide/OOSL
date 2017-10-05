#pragma once

#ifndef OOSL_TYPE_GRAMMAR_H
#define OOSL_TYPE_GRAMMAR_H

#include "general_grammar.h"

#include "../type/modified_type.h"
#include "../type/pointer_type.h"

namespace oosl{
	namespace lexer{
		class expression_grammar;

		class type_specifier_grammar : public typed_grammar< oosl::type::object::attribute>{
		public:
			typedef typed_grammar< oosl::type::object::attribute> base_type;
			typedef oosl::type::object::attribute type_attribute_type;
			typedef boost::spirit::qi::symbols<char, type_attribute_type> specifier_symbols_type;

			type_specifier_grammar();

			static type_attribute_type combine(type_attribute_type left, type_attribute_type right);

		protected:
			specifier_symbols_type static_symbols_;
			specifier_symbols_type thread_local_symbols_;
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

		class decltype_grammar : public grammar{
		public:
			typedef std::shared_ptr<expression_grammar> expression_grammar_type;

			decltype_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar_type expression_;
		};

		class type_grammar : public grammar{
		public:
			type_grammar();

		protected:
			primitive_type_grammar primitive_type_;
			decltype_grammar decltype_;
		};

		class modified_type_grammar : public grammar{
		public:
			modified_type_grammar();

			static node_ptr_type create_ref(node_ptr_type value);

			static node_ptr_type create_pointer(node_ptr_type value, const std::string &dim);

		protected:
			rule_type ref_;
			rule_type pointer_;
			type_grammar type_;
			identifier_compatible_grammar identifier_compatible_;
		};

		class full_modified_type_grammar : public grammar{
		public:
			full_modified_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			rule_type rval_ref_;
			type_grammar type_;
			identifier_compatible_grammar identifier_compatible_;
			modified_type_grammar modified_type_;
		};

		class typename_type_grammar : public grammar{
		public:
			typename_type_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			type_grammar type_;
			identifier_compatible_grammar identifier_compatible_;
			modified_type_grammar modified_type_;
		};
	}
}

#endif /* !OOSL_TYPE_GRAMMAR_H */
