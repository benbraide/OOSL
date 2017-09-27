#pragma once

#ifndef OOSL_CONSTANT_GRAMMAR_H
#define OOSL_CONSTANT_GRAMMAR_H

#include "constant_ast.h"

namespace oosl{
	namespace lexer{
		class constant_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(constant)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(constant)(), skipper> constant_rule_type;
			typedef boost::spirit::qi::symbols<char, constant_type> constant_symbols_type;

			constant_grammar();

		protected:
			constant_rule_type start_;
			constant_symbols_type constant_symbols_;
		};
	}
}

#endif /* !OOSL_CONSTANT_GRAMMAR_H */
