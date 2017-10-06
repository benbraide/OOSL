#pragma once

#ifndef OOSL_EXPRESSION_GRAMMAR_H
#define OOSL_EXPRESSION_GRAMMAR_H

#include "constant_grammar.h"
#include "general_grammar.h"
#include "type_grammar.h"

namespace oosl{
	namespace lexer{
		class range_grammar : public grammar{
		public:
			range_grammar();

			static node_ptr_type create(node_ptr_type left, node_ptr_type right);

			static entry_type *eval(node_ptr_type left, node_ptr_type right);

			template <typename value_type>
			static entry_type *eval(value_type from, value_type to){
				return nullptr;
			}

			static std::size_t count(node_ptr_type left, node_ptr_type right);

			template <typename value_type>
			static std::size_t count(value_type from, value_type to){
				return ((from < to) ? ((to - from) + 1) : ((from - to) + 1));
			}

		protected:
			integer_literal_grammar integer_literal_;
		};

		class grouped_expression_grammar;
		class list_expression_grammar;

		class non_operator_term_grammar : public grammar{
		public:
			typedef std::shared_ptr<grouped_expression_grammar> grouped_expression_grammar_type;
			typedef std::shared_ptr<list_expression_grammar> list_expression_grammar_type;

			non_operator_term_grammar();

		protected:
			literal_grammar literal_;
			constant_grammar constant_;
			identifier_compatible_grammar identifier_compatible_;
			system_call_grammar system_call_;
			grouped_expression_grammar_type grouped_expression_;
			list_expression_grammar_type list_expression_;
			type_grammar type_;
		};

		class term_grammar : public grammar{
		public:
			term_grammar();

		protected:
			non_operator_term_grammar non_operator_term_;
		};

		class non_list_expression_grammar : public grammar{
		public:
			non_list_expression_grammar();

		protected:

		};

		class expression_grammar : public grammar{
		public:
			expression_grammar();

		protected:

		};

		class grouped_expression_grammar : public grammar{
		public:
			grouped_expression_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar expression_;
		};

		class list_expression_grammar : public grammar{
		public:
			list_expression_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:

		};
	}
}

#endif /* !OOSL_EXPRESSION_GRAMMAR_H */
