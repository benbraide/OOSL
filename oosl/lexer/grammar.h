#pragma once

#ifndef OOSL_GRAMMAR_H
#define OOSL_GRAMMAR_H

#include <string>
#include <unordered_map>

#include "../common/controller.h"

#include "rule_id.h"
#include "skipper.h"

namespace oosl{
	namespace lexer{
		class grammar : public boost::spirit::qi::grammar<const char *, std::string(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef rule_id rule_id_type;

			typedef boost::spirit::qi::rule<iterator_type, std::string(), skipper> rule_type;
			typedef boost::spirit::qi::symbols<char, rule_id_type> symbols_type;
			typedef oosl::type::id type_id_type;

			typedef std::unordered_map<rule_id_type, rule_type> rule_list_type;

			grammar();

		protected:
			void initialize_symbols_();

			rule_type start_;
			symbols_type symbols_;
			rule_list_type rule_list_;
		};
	}
}

#endif /* !OOSL_GRAMMAR_H */
