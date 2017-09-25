#include "skipper.h"

oosl::lexer::skipper::skipper()
	: skipper::base_type(start_, "skipper"){
	using namespace boost::spirit;

	line_comment_ = qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol;
	block_comment_ = qi::lit("/*") > *(qi::char_ - qi::lit("*/")) > qi::lit("*/");
	start_ = qi::space | line_comment_ | block_comment_;
}
