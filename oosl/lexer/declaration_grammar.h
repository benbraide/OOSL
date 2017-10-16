#pragma once

#ifndef OOSL_DECLARATION_GRAMMAR_H
#define OOSL_DECLARATION_GRAMMAR_H

#include "expression_grammar.h"
#include "../node/declaration_node.h"

namespace oosl{
	namespace lexer{
		class declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			declaration_grammar();

			static node_ptr_type create(boost::optional<storage_attribute_type> attributes, node_ptr_type type, node_ptr_type id, boost::optional<node_ptr_type> init);

			static node_ptr_type create_multiple(node_ptr_type decl, node_ptr_type id, boost::optional<node_ptr_type> init);

		protected:
			rule_type base_;
			type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			identifier_compatible_grammar identifier_compatible_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};

		class auto_or_ref_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			auto_or_ref_declaration_grammar();

			static node_ptr_type create(boost::optional<storage_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init);

			static node_ptr_type create_multiple(node_ptr_type decl, node_ptr_type id, node_ptr_type init);

		protected:
			rule_type base_;
			ref_type_grammar ref_type_;
			auto_type_grammar auto_type_;
			ref_auto_type_grammar ref_auto_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			non_list_expression_grammar non_list_expression_;
			storage_specifier_grammar storage_specifier_;
		};

		class parameter_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			parameter_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

		protected:
			parameter_type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
		};

		class parameter_init_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			parameter_init_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id, node_ptr_type init);

		protected:
			parameter_type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			non_list_expression_grammar non_list_expression_;
		};

		class variadic_declaration_grammar : public grammar{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			variadic_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

		protected:
			variadic_type_grammar type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
		};
	}
}

#endif /* !OOSL_DECLARATION_GRAMMAR_H */
