#pragma once

#ifndef OOSL_OPERATOR_AST_H
#define OOSL_OPERATOR_AST_H

#include "ast.h"
#include "../common/operator_id.h"

#define OOSL_AST_OPERATOR_NAME(name) OOSL_AST_NAME(OOSL_AST_JOIN(name, _operator))
#define OOSL_AST_OPERATOR_QNAME(name) OOSL_AST_QNAME(OOSL_AST_JOIN(name, _operator))

#define OOSL_AST_DEFINE_OPERATOR(name)			\
struct OOSL_AST_OPERATOR_NAME(name){			\
	oosl::common::operator_id value;			\
};

#define OOSL_AST_DEFINE_UNKNOWN_OPERATOR(name)	\
struct OOSL_AST_OPERATOR_NAME(name){			\
	std::string value;							\
};

#define OOSL_AST_ADAPT_OPERATOR(name)			\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_OPERATOR_QNAME(name),				\
	(oosl::common::operator_id, value)			\
)

#define OOSL_AST_ADAPT_UNKNOWN_OPERATOR(name)	\
BOOST_FUSION_ADAPT_STRUCT(						\
	OOSL_AST_OPERATOR_QNAME(name),				\
	(std::string, value)						\
)

namespace oosl{
	namespace lexer{
		OOSL_AST_DEFINE_OPERATOR(member_access);

		OOSL_AST_DEFINE_OPERATOR(factor);

		OOSL_AST_DEFINE_OPERATOR(addition);

		OOSL_AST_DEFINE_OPERATOR(shift);

		OOSL_AST_DEFINE_OPERATOR(relation);

		OOSL_AST_DEFINE_OPERATOR(equality);

		OOSL_AST_DEFINE_OPERATOR(bitwise_and);

		OOSL_AST_DEFINE_OPERATOR(bitwise_xor);

		OOSL_AST_DEFINE_OPERATOR(bitwise_or);

		OOSL_AST_DEFINE_OPERATOR(and);

		OOSL_AST_DEFINE_OPERATOR(or);

		OOSL_AST_DEFINE_OPERATOR(comma);

		OOSL_AST_DEFINE_OPERATOR(ternary);

		OOSL_AST_DEFINE_OPERATOR(assignment);

		OOSL_AST_DEFINE_UNKNOWN_OPERATOR(unknown);
	}
}

OOSL_AST_ADAPT_OPERATOR(member_access)

OOSL_AST_ADAPT_OPERATOR(factor)

OOSL_AST_ADAPT_OPERATOR(addition)

OOSL_AST_ADAPT_OPERATOR(shift)

OOSL_AST_ADAPT_OPERATOR(relation)

OOSL_AST_ADAPT_OPERATOR(equality)

OOSL_AST_ADAPT_OPERATOR(bitwise_and)

OOSL_AST_ADAPT_OPERATOR(bitwise_xor)

OOSL_AST_ADAPT_OPERATOR(bitwise_or)

OOSL_AST_ADAPT_OPERATOR(and)

OOSL_AST_ADAPT_OPERATOR(or)

OOSL_AST_ADAPT_OPERATOR(comma)

OOSL_AST_ADAPT_OPERATOR(ternary)

OOSL_AST_ADAPT_OPERATOR(assignment)

OOSL_AST_ADAPT_UNKNOWN_OPERATOR(unknown)

#endif /* !OOSL_OPERATOR_AST_H */
