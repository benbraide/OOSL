#pragma once

#ifndef OOSL_EXPRESSION_AST_H
#define OOSL_EXPRESSION_AST_H

#include "general_ast.h"

namespace oosl{
	namespace lexer{
		struct OOSL_AST_NAME(range){
			OOSL_AST_NAME(integer) from;
			OOSL_AST_NAME(integer) to;
		};

		struct OOSL_AST_NAME(expression){

		};

		struct OOSL_AST_NAME(grouped_expression){

		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(range),
	(OOSL_AST_QNAME(integer), from)
	(OOSL_AST_QNAME(integer), to)
)

#endif /* !OOSL_EXPRESSION_AST_H */
