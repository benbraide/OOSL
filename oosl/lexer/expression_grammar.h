#pragma once

#ifndef OOSL_EXPRESSION_GRAMMAR_H
#define OOSL_EXPRESSION_GRAMMAR_H

#include "expression_ast.h"
#include "constant_grammar.h"
#include "general_grammar.h"

namespace oosl{
	namespace lexer{
		class range_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(range)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(range)(), skipper> start_rule_type;

			range_grammar();

		protected:
			start_rule_type start_;
			integer_literal_grammar integer_literal_;
		};

		class expression_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(expression)(), skipper>{

		};

		class grouped_expression_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(grouped_expression)(), skipper>{

		};
	}
}

#endif /* !OOSL_EXPRESSION_GRAMMAR_H */
