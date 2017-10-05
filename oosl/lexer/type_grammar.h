#pragma once

#ifndef OOSL_TYPE_GRAMMAR_H
#define OOSL_TYPE_GRAMMAR_H

#include "grammar.h"

namespace oosl{
	namespace lexer{
		class type_specifier_grammar : public typed_grammar< oosl::type::object::attribute>{
		public:
			typedef typed_grammar< oosl::type::object::attribute> base_type;
			typedef oosl::type::object::attribute type_attribute_type;
			typedef boost::spirit::qi::symbols<char, type_attribute_type> specifier_symbols_type;

			type_specifier_grammar();

			static type_attribute_type combine(type_attribute_type left, type_attribute_type right);

		protected:
			specifier_symbols_type static_symbols_;
			specifier_symbols_type thread_local_symbols_;
		};

		class primitive_type_grammar : public grammar{
		public:
			typedef oosl::type::id type_id_type;
			typedef boost::spirit::qi::symbols<char, type_id_type> type_symbols_type;

			primitive_type_grammar();

			static node_ptr_type create(type_id_type value);

		protected:
			type_symbols_type type_symbols_;
		};
	}
}

#endif /* !OOSL_TYPE_GRAMMAR_H */
