#pragma once

#ifndef OOSL_STATEMENT_GRAMMAR_H
#define OOSL_STATEMENT_GRAMMAR_H

#include "expression_grammar.h"

namespace oosl{
	namespace lexer{
		class variadic_declaration_grammar : public grammar{
		public:
			variadic_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

		protected:
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			variadic_type_grammar variadic_type_;
		};

		class parameter_declaration_grammar : public grammar{
		public:
			parameter_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, boost::optional<node_ptr_type> id);

			static node_ptr_type create_init(node_ptr_type value, boost::optional<node_ptr_type> init);

		protected:
			rule_type base_;
			type_grammar type_;
			full_modified_type_grammar full_modified_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			identifier_compatible_grammar identifier_compatible_;
			non_list_expression_grammar non_list_expression_;
		};

		class explicit_declaration_grammar : public grammar{
		public:
			explicit_declaration_grammar();

			static node_ptr_type create(node_ptr_type type, node_ptr_type id);

			static node_ptr_type create_init(node_ptr_type value, boost::optional<node_ptr_type> init);

		protected:
			rule_type required_;
			rule_type optional_;

			type_grammar type_;
			typename_type_grammar typename_type_;
			specified_type_grammar specified_type_;
			identifier_or_placeholder_grammar identifier_or_placeholder_;
			identifier_compatible_grammar identifier_compatible_;
			non_list_expression_grammar non_list_expression_;
		};

		class implicit_declaration_grammar : public grammar{
		public:
			implicit_declaration_grammar();

		protected:

		};

		class declaration_grammar : public grammar{
		public:
			declaration_grammar();

		protected:

		};
	}
}

#endif /* !OOSL_STATEMENT_GRAMMAR_H */
