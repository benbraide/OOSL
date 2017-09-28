#pragma once

#ifndef OOSL_GENERAL_GRAMMAR_H
#define OOSL_GENERAL_GRAMMAR_H

#include "general_ast.h"
#include "literal_grammar.h"

namespace oosl{
	namespace lexer{
		class identifier_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(identifier)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(identifier)(), skipper> start_rule_type;

			identifier_grammar();

		protected:
			start_rule_type start_;
		};

		class placeholder_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(placeholder)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(placeholder)(), skipper> start_rule_type;

			placeholder_grammar();

		protected:
			start_rule_type start_;
			string_literal_grammar string_literal_;
			identifier_grammar identifier_;
		};

		class identifier_compatible_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(identifier_compatible)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(identifier_compatible)(), skipper> start_rule_type;

			identifier_compatible_grammar();

		protected:
			start_rule_type start_;
			placeholder_grammar placeholder_;
			identifier_grammar identifier_;
		};

		class global_qualified_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(global_qualified)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(global_qualified)(), skipper> start_rule_type;

			global_qualified_grammar();

		protected:
			start_rule_type start_;
			identifier_compatible_grammar identifier_;
		};

		class recursive_qualified_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(recursive_qualified)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(recursive_qualified)(), skipper> start_rule_type;

			recursive_qualified_grammar();

		protected:
			start_rule_type start_;
			identifier_compatible_grammar identifier_;
		};

		class relative_qualified_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(relative_qualified)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(relative_qualified)(), skipper> start_rule_type;

			relative_qualified_grammar();

		protected:
			start_rule_type start_;
			identifier_compatible_grammar identifier_;
			recursive_qualified_grammar recursive_qualified_;
			global_qualified_grammar global_qualified_;
		};

		class qualified_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(qualified)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(qualified)(), skipper> start_rule_type;

			qualified_grammar();

		protected:
			start_rule_type start_;
			relative_qualified_grammar relative_qualified_;
			global_qualified_grammar global_qualified_;
		};

		class full_identifier_compatible_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(full_identifier_compatible)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(full_identifier_compatible)(), skipper> start_rule_type;

			full_identifier_compatible_grammar();

		protected:
			start_rule_type start_;
			qualified_grammar qualified_;
			identifier_compatible_grammar identifier_;
		};

		class system_call_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_NAME(system_call)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_NAME(system_call)(), skipper> start_rule_type;

			system_call_grammar();

		protected:
			start_rule_type start_;
		};
	}
}

#endif /* !OOSL_GENERAL_GRAMMAR_H */
