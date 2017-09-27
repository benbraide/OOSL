#include "general_grammar.h"

oosl::lexer::identifier_grammar::identifier_grammar()
	: identifier_grammar::base_type(start_, "OOSL_IDENITIFIER"){
	using namespace boost::spirit;
	start_ %= qi::lexeme[qi::char_("$_A-Za-z") >> *qi::char_("$_A-Za-z0-9")];
}

oosl::lexer::global_qualified_grammar::global_qualified_grammar()
	: global_qualified_grammar::base_type(start_, "OOSL_GLOBAL_QUALIFIED"){
	start_ %= "::" >> identifier_;
}

oosl::lexer::recursive_qualified_grammar::recursive_qualified_grammar()
	: recursive_qualified_grammar::base_type(start_, "OOSL_RECURSIVE_QUALIFIED"){
	start_ %= identifier_ >> -("::" >> (*this | identifier_));
}

oosl::lexer::relative_qualified_grammar::relative_qualified_grammar()
	: relative_qualified_grammar::base_type(start_, "OOSL_RELATIVE_QUALIFIED"){
	start_ %= (global_qualified_ | identifier_) >> "::" >> recursive_qualified_;
}

oosl::lexer::qualified_grammar::qualified_grammar()
	: qualified_grammar::base_type(start_, "OOSL_QUALIFIED"){
	start_ = relative_qualified_ | global_qualified_;
}
