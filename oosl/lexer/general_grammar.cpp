#include "general_grammar.h"
#include "expression_grammar.h"

oosl::lexer::identifier_grammar::identifier_grammar()
	: identifier_grammar::base_type(start_, "OOSL_IDENITIFIER"){
	using namespace boost::spirit;
	start_ %= qi::lexeme[qi::char_("$_A-Za-z") >> *qi::char_("$_A-Za-z0-9")];
}

oosl::lexer::placeholder_grammar::placeholder_grammar()
	: placeholder_grammar::base_type(start_, "OOSL_PLACEHOLDER"){
	using namespace boost::spirit;
	start_ %= "__placeholder" > qi::lit("(") > (string_literal_ | identifier_) > ")";
}

oosl::lexer::identifier_compatible_grammar::identifier_compatible_grammar()
	: identifier_compatible_grammar::base_type(start_, "OOSL_IDENITIFIER_COMPATIBLE"){
	using namespace boost::spirit;
	start_ %= placeholder_ | identifier_;
}

oosl::lexer::global_qualified_grammar::global_qualified_grammar()
	: global_qualified_grammar::base_type(start_, "OOSL_GLOBAL_QUALIFIED"){
	start_ %= "::" >> identifier_;
}

oosl::lexer::recursive_qualified_grammar::recursive_qualified_grammar()
	: recursive_qualified_grammar::base_type(start_, "OOSL_RECURSIVE_QUALIFIED"){
	start_ %= identifier_ >> "::" >> (*this | identifier_);
}

oosl::lexer::relative_qualified_grammar::relative_qualified_grammar()
	: relative_qualified_grammar::base_type(start_, "OOSL_RELATIVE_QUALIFIED"){
	start_ %= (global_qualified_ | identifier_) >> "::" >> (recursive_qualified_ | identifier_);
}

oosl::lexer::qualified_grammar::qualified_grammar()
	: qualified_grammar::base_type(start_, "OOSL_QUALIFIED"){
	start_ = relative_qualified_ | global_qualified_;
}

oosl::lexer::full_identifier_compatible_grammar::full_identifier_compatible_grammar()
	: full_identifier_compatible_grammar::base_type(start_, "OOSL_FULL_IDENITIFIER_COMPATIBLE"){
	using namespace boost::spirit;
	start_ %= qualified_ | identifier_;
}

oosl::lexer::system_call_grammar::system_call_grammar()
	: system_call_grammar::base_type(start_, "OOSL_SYSTEM_CALL"){
	using namespace boost::spirit;
	start_ %= "__call" > qi::lit("(") > qi::uint_ > ")";
}
