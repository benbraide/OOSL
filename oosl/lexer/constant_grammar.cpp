#include "constant_grammar.h"

oosl::lexer::constant_grammar::constant_grammar()
	: constant_grammar::base_type(start_, "OOSL_CONSTANT"){
	start_ %= constant_symbols_;
	constant_symbols_.add
		("false", constant_type::false_)
		("true", constant_type::true_)
		("indeterminate", constant_type::indeterminate)
		("nullptr", constant_type::nullptr_)
		("NaN", constant_type::nan_)
		("undefined", constant_type::undefined);
}
