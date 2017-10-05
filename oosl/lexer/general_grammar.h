#pragma once

#ifndef OOSL_GENERAL_GRAMMAR_H
#define OOSL_GENERAL_GRAMMAR_H

#include "literal_grammar.h"

namespace oosl{
	namespace lexer{
		class identifier_grammar : public grammar{
		public:
			identifier_grammar();

			static node_ptr_type create(const std::string &value);

			static entry_type *lookup(const std::string &key);
		};

		class placeholder_grammar : public grammar{
		public:
			placeholder_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			string_literal_grammar string_literal_;
			identifier_grammar identifier_;
		};

		class identifier_or_placeholder_grammar : public grammar{
		public:
			identifier_or_placeholder_grammar();

		protected:
			placeholder_grammar placeholder_;
			identifier_grammar identifier_;
		};

		class absolute_qualified_grammar : public grammar{
		public:
			absolute_qualified_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			identifier_or_placeholder_grammar identifier_;
		};

		class relative_qualified_grammar : public grammar{
		public:
			relative_qualified_grammar();

			static node_ptr_type create(node_ptr_type left, node_ptr_type right);

		protected:
			absolute_qualified_grammar absolute_qualified_;
			identifier_or_placeholder_grammar identifier_;
		};

		class qualified_grammar : public grammar{
		public:
			qualified_grammar();

		protected:
			relative_qualified_grammar relative_qualified_;
			absolute_qualified_grammar absolute_qualified_;
		};

		class identifier_compatible_grammar : public grammar{
		public:
			identifier_compatible_grammar();

		protected:
			qualified_grammar qualified_;
			identifier_or_placeholder_grammar identifier_;
		};

		class system_call_grammar : public grammar{
		public:
			system_call_grammar();

			static node_ptr_type create(unsigned int value);
		};
	}
}

#endif /* !OOSL_GENERAL_GRAMMAR_H */
