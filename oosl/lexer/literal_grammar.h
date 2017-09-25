#pragma once

#ifndef OOSL_LITERAL_GRAMMAR_H
#define OOSL_LITERAL_GRAMMAR_H

#include "skipper.h"
#include "literal_ast.h"

namespace oosl{
	namespace lexer{
		class numeric_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(num)(), skipper>{
		public:
			typedef const char *iterator_type;

			template <typename value_type>
			struct real_policies : boost::spirit::qi::real_policies<value_type>{
				static bool const expect_dot			= true;
				static bool const allow_leading_dot		= true;
				static bool const allow_trailing_dot	= true;
			};

			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(rad)(), skipper> rad_rule_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(integer)(), skipper> integer_rule_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(real)(), skipper> real_rule_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(num)(), skipper> num_rule_type;

			typedef boost::spirit::qi::real_parser<long double, real_policies<long double>> real_parser_type;
			typedef boost::spirit::qi::symbols<char, numeric_suffix_type> suffix_symbols_type;

			numeric_literal_grammar();

		protected:
			num_rule_type start_;
			real_parser_type real_parser_;

			rad_rule_type rad_;
			integer_rule_type integer_;
			real_rule_type real_;

			suffix_symbols_type suffix_symbols_;
		};
	}
}

#endif /* !OOSL_LITERAL_GRAMMAR_H */
