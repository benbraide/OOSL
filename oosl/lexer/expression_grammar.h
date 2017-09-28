#pragma once

#ifndef OOSL_EXPRESSION_GRAMMAR_H
#define OOSL_EXPRESSION_GRAMMAR_H

#include "expression_ast.h"

namespace oosl{
	namespace lexer{
		class expression_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(expression)(), skipper>{

		};

		class grouped_expression_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(grouped_expression)(), skipper>{

		};
	}
}

#endif /* !OOSL_EXPRESSION_GRAMMAR_H */
