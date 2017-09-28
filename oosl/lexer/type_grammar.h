#pragma once

#ifndef OOSL_TYPE_GRAMMAR_H
#define OOSL_TYPE_GRAMMAR_H

#include "type_ast.h"

namespace oosl{
	namespace lexer{
		class type_specifier_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(type_specifier)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(type_specifier)(), skipper> start_rule_type;
			typedef boost::spirit::qi::symbols<char, type_specifier> specifier_symbols_type;

			type_specifier_grammar();

		protected:
			start_rule_type start_;
			specifier_symbols_type static_symbols_;
			specifier_symbols_type thread_local_symbols_;
		};

		class primitive_type_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(primitive_type)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(primitive_type)(), skipper> start_rule_type;
			typedef boost::spirit::qi::symbols<char, primitive_type> type_symbols_type;

			primitive_type_grammar();

		protected:
			start_rule_type start_;
			type_symbols_type type_symbols_;
		};
	}
}

#endif /* !OOSL_TYPE_GRAMMAR_H */
