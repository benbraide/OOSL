#pragma once

#ifndef OOSL_STATEMENT_GRAMMAR_H
#define OOSL_STATEMENT_GRAMMAR_H

#include "expression_grammar.h"

namespace oosl{
	namespace lexer{
		class parameter_declaration_grammar : public grammar{
		public:
			typedef std::vector<node_ptr_type> node_ptr_list_type;

			parameter_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

		protected:
			parameter_type_grammar parameter_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
		};

		class variadic_declaration_grammar : public grammar{
		public:
			variadic_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

		protected:
			variadic_type_grammar variadic_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
		};

		class parameter_init_declaration_grammar : public grammar{
		public:
			parameter_init_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, node_ptr_type init);

		protected:
			parameter_declaration_grammar parameter_declaration_;
			non_list_expression_grammar non_list_expression_;
		};

		class declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute type_attribute_type;

			declaration_grammar();

			static node_ptr_type create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id);

			static node_ptr_type create_init(node_ptr_type value, boost::optional<node_ptr_type> init);

		protected:
			rule_type base_;
			type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			identifier_compatible_grammar identifier_compatible_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};

		class declaration_extension_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute type_attribute_type;

			declaration_extension_grammar();

			static node_ptr_type create(node_ptr_type type, node_ptr_type id);

			static node_ptr_type create_init(node_ptr_type value, boost::optional<node_ptr_type> init);

		protected:
			rule_type base_;
			type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};

		class ref_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute type_attribute_type;

			ref_declaration_grammar();

			static node_ptr_type create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init);

		protected:
			rule_type base_;
			ref_type_grammar ref_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};

		class auto_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute type_attribute_type;

			auto_declaration_grammar();

			static node_ptr_type create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init);

		protected:
			rule_type base_;
			auto_type_grammar auto_type_;
			ref_auto_type_grammar ref_auto_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};
	}
}

#endif /* !OOSL_STATEMENT_GRAMMAR_H */
