#pragma once

#ifndef OOSL_LITERAL_GRAMMAR_H
#define OOSL_LITERAL_GRAMMAR_H

#include "literal_ast.h"

namespace oosl{
	namespace lexer{
		class rad_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(rad)(), skipper>{
		public:
			typedef const char *iterator_type;

			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(rad)(), skipper> start_rule_type;
			typedef boost::spirit::qi::symbols<char, numeric_suffix_type> suffix_symbols_type;

			rad_literal_grammar();

		protected:
			start_rule_type start_;
			suffix_symbols_type suffix_symbols_;
		};

		class integer_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(integer)(), skipper>{
		public:
			typedef const char *iterator_type;

			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(integer)(), skipper> start_rule_type;
			typedef boost::spirit::qi::symbols<char, numeric_suffix_type> suffix_symbols_type;

			integer_literal_grammar();

		protected:
			start_rule_type start_;
			suffix_symbols_type suffix_symbols_;
		};

		class real_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(real)(), skipper>{
		public:
			typedef const char *iterator_type;

			template <typename value_type>
			struct real_policies : boost::spirit::qi::real_policies<value_type>{
				static bool const expect_dot = true;
				static bool const allow_leading_dot = true;
				static bool const allow_trailing_dot = true;
			};

			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(real)(), skipper> start_rule_type;
			typedef boost::spirit::qi::real_parser<long double, real_policies<long double>> real_parser_type;
			typedef boost::spirit::qi::symbols<char, numeric_suffix_type> suffix_symbols_type;

			real_literal_grammar();

		protected:
			start_rule_type start_;
			real_parser_type real_parser_;
			suffix_symbols_type suffix_symbols_;
		};

		class numeric_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(num)(), skipper>{
		public:
			typedef const char *iterator_type;

			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(num)(), skipper> start_rule_type;

			numeric_literal_grammar();

		protected:
			start_rule_type start_;
			rad_literal_grammar rad_;
			integer_literal_grammar integer_;
			real_literal_grammar real_;
		};

		template <class ast_type, char quote>
		class basic_quote_literal_grammar : public boost::spirit::qi::grammar<const char *, ast_type(), skipper>{
		public:
			typedef const char *iterator_type;

			typedef boost::spirit::qi::rule<iterator_type, ast_type(), skipper> quote_rule_type;
			typedef boost::spirit::qi::symbols<char, string_prefix_type> prefix_symbols_type;
			typedef boost::spirit::qi::symbols<char, char> escape_symbols_type;

			basic_quote_literal_grammar()
				: basic_quote_literal_grammar::base_type(start_, "OOSL_BASIC_QUOTE_LITERAL"){
				using namespace boost::spirit;

				non_escaped_	%= qi::lexeme['@' >> -prefix_symbols_ >> quote >> qi::as_string[*(~qi::char_(quote))] >> quote];
				start_			%= non_escaped_ | qi::lexeme[-prefix_symbols_ >> quote >> qi::as_string[*(escape_symbols_ | ("\\x" >> qi::hex) | ~qi::char_(quote))] >> quote];

				prefix_symbols_.add
					("L", string_prefix_type::wide);

				escape_symbols_.add
					("\\0", '\0')
					("\\a", '\a')
					("\\b", '\b')
					("\\f", '\f')
					("\\n", '\n')
					("\\r", '\r')
					("\\t", '\t')
					("\\v", '\v')
					("\\?", '\?')
					("\\\"", '\"')
					("\\\'", '\'')
					("\\`", '`')
					("\\\\", '\\');
			}

		protected:
			quote_rule_type start_;
			quote_rule_type non_escaped_;

			prefix_symbols_type prefix_symbols_;
			escape_symbols_type escape_symbols_;
		};

		using character_literal_grammar = basic_quote_literal_grammar<OOSL_AST_NAME(character), '\''>;
		using string_literal_grammar = basic_quote_literal_grammar<OOSL_AST_NAME(string), '\"'>;
		using raw_literal_grammar = basic_quote_literal_grammar<OOSL_AST_NAME(raw), '`'>;

		class quote_literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(quote)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(quote)(), skipper> quote_rule_type;

			quote_literal_grammar();

		protected:
			quote_rule_type start_;
			character_literal_grammar character_;
			string_literal_grammar string_;
			raw_literal_grammar raw_;
		};

		class literal_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(lit)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(lit)(), skipper> lit_rule_type;

			literal_grammar();

		protected:
			lit_rule_type start_;
			numeric_literal_grammar numeric_;
			quote_literal_grammar quote_;
		};
	}
}

#endif /* !OOSL_LITERAL_GRAMMAR_H */
