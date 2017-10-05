#pragma once

#ifndef OOSL_CONSTANT_GRAMMAR_H
#define OOSL_CONSTANT_GRAMMAR_H

#include "grammar.h"

namespace oosl{
	namespace lexer{
		class constant_grammar : public grammar{
		public:
			enum class constant_type{
				nil,
				false_,
				true_,
				indeterminate,
				nullptr_,
				nan_,
				undefined,
			};

			typedef boost::spirit::qi::symbols<char, constant_type> constant_symbols_type;

			constant_grammar();

			static node_ptr_type create(constant_type value);

		protected:
			constant_symbols_type constant_symbols_;
		};
	}
}

#endif /* !OOSL_CONSTANT_GRAMMAR_H */
