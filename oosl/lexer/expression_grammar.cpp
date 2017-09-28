#include "expression_grammar.h"

oosl::lexer::range_grammar::range_grammar()
	: range_grammar::base_type(start_, "OOSL_RANGE"){
	using namespace boost::spirit;
	start_ %= integer_literal_ >> "..." >> integer_literal_;
}
