#include "operator_grammar.h"

oosl::lexer::unknown_operator_grammar::unknown_operator_grammar()
	: unknown_operator_grammar::base_type(start_, "OOSL_UNKNOWN_OPERATOR"){
	using namespace boost::spirit;
	start_ %= qi::as_string[*(qi::punct - qi::char_(OOSL_AST_OPERATOR_EXCLUDES))];
}
