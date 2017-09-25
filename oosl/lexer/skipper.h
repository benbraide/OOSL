#pragma once

#ifndef OOSL_SKIPPER_H
#define OOSL_SKIPPER_H

#include <boost/spirit/include/qi.hpp>

namespace oosl{
	namespace lexer{
		class skipper : public boost::spirit::qi::grammar<const char *>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type> rule_type;

			skipper();

		protected:
			rule_type start_;
			rule_type line_comment_;
			rule_type block_comment_;
		};
	}
}

#endif /* !OOSL_SKIPPER_H */
