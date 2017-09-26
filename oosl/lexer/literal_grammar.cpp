#include "literal_grammar.h"

oosl::lexer::numeric_literal_grammar::numeric_literal_grammar()
	: numeric_literal_grammar::base_type(start_, "OOSL_NUMERIC_LITERAL"){
	using namespace boost::spirit;

	integer_ %= qi::long_long;
	real_ %= real_parser_;
	rad_ %= qi::lexeme[qi::int_ >> 'r' >> qi::as_string[+qi::alnum]];

	start_ %= (rad_ | real_ | integer_) >> qi::no_skip[-suffix_symbols_];

	suffix_symbols_.add
	("i8", numeric_suffix_type::i8)
		("ui8", numeric_suffix_type::ui8)
		("i16", numeric_suffix_type::i16)
		("ui16", numeric_suffix_type::ui16)
		("i32", numeric_suffix_type::i32)
		("ui32", numeric_suffix_type::ui32)
		("i64", numeric_suffix_type::i64)
		("ui64", numeric_suffix_type::ui64)
		("i128", numeric_suffix_type::i128)
		("ui128", numeric_suffix_type::ui128)
		("f32", numeric_suffix_type::f32)
		("f64", numeric_suffix_type::f64)
		("f128", numeric_suffix_type::f128);
}

oosl::lexer::quote_literal_grammar::quote_literal_grammar()
	: quote_literal_grammar::base_type(start_, "OOSL_QUOTE_LITERAL"){
	start_ %= character_ | string_ | raw_;
}

oosl::lexer::literal_grammar::literal_grammar()
	: literal_grammar::base_type(start_, "OOSL_LITERAL"){
	start_ %= numeric_ | quote_;
}
