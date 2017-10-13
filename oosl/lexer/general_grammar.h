#pragma once

#ifndef OOSL_GENERAL_GRAMMAR_H
#define OOSL_GENERAL_GRAMMAR_H

#include "literal_grammar.h"

namespace oosl{
	namespace lexer{
		class expression_grammar;
		class non_list_expression_grammar;
		class variadic_expression_grammar;

		class expression_list_grammar : public grammar{
		public:
			typedef std::shared_ptr<non_list_expression_grammar> expression_grammar_type;
			typedef std::shared_ptr<variadic_expression_grammar> variadic_expression_grammar_type;
			typedef std::vector<node_ptr_type> node_ptr_list_type;

			expression_list_grammar();

			static node_ptr_type create(const node_ptr_list_type &value);

		protected:
			expression_grammar_type expression_;
			variadic_expression_grammar_type variadic_expression_;
		};

		class keyword_grammar : public grammar{
		public:
			typedef boost::spirit::qi::symbols<char> symbols_type;

			keyword_grammar();

			static node_ptr_type create();

		protected:
			symbols_type symbols_;
		};

		class identifier_grammar : public grammar{
		public:
			typedef boost::spirit::qi::rule<iterator_type, std::string(), skipper> custom_rule_type;

			identifier_grammar();

			static node_ptr_type create(const std::string &value);

			static entry_type *lookup(const std::string &key);

		protected:
			custom_rule_type base_;
			keyword_grammar keyword_;
		};

		class placeholder_grammar : public grammar{
		public:
			typedef std::shared_ptr<expression_grammar> expression_grammar_type;

			placeholder_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar_type expression_;
		};

		class identifier_or_placeholder_grammar : public grammar{
		public:
			identifier_or_placeholder_grammar();

		protected:
			placeholder_grammar placeholder_;
			identifier_grammar identifier_;
		};

		class absolute_qualified_grammar : public grammar{
		public:
			absolute_qualified_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			identifier_or_placeholder_grammar identifier_;
		};

		class relative_qualified_grammar : public grammar{
		public:
			relative_qualified_grammar();

			static node_ptr_type create(node_ptr_type left, node_ptr_type right);

		protected:
			absolute_qualified_grammar absolute_qualified_;
			identifier_or_placeholder_grammar identifier_;
		};

		class qualified_grammar : public grammar{
		public:
			qualified_grammar();

		protected:
			relative_qualified_grammar relative_qualified_;
			absolute_qualified_grammar absolute_qualified_;
		};

		class identifier_compatible_grammar : public grammar{
		public:
			identifier_compatible_grammar();

		protected:
			qualified_grammar qualified_;
			identifier_or_placeholder_grammar identifier_;
		};

		class system_call_grammar : public grammar{
		public:
			system_call_grammar();

			static node_ptr_type create(unsigned int value, boost::optional<node_ptr_type> args);

		protected:
			expression_list_grammar expression_list_;
		};
	}
}

#endif /* !OOSL_GENERAL_GRAMMAR_H */
