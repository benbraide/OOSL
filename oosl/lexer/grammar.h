#pragma once

#ifndef OOSL_GRAMMAR_H
#define OOSL_GRAMMAR_H

#include <string>

#include <boost/optional.hpp>
#include "boost/spirit/include/phoenix.hpp"
#include <boost/fusion/adapted.hpp>

#include "skipper.h"

#include "../type/type_object.h"
#include "../node/inplace_node.h"
#include "../common/structures.h"
#include "../driver/driver_object.h"
#include "../storage/temporary_storage.h"

#define OOSL_AST_OPERATOR_EXCLUDES "()[]{}\"'`\\,;:$_"

namespace oosl{
	namespace lexer{
		template <class type>
		class typed_grammar : public boost::spirit::qi::grammar<const char *, type(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, type(), skipper> rule_type;

			typedef oosl::common::output_writer output_writer_type;
			typedef output_writer_type::write_option_type write_option_type;

			typedef oosl::node::id node_id_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::node::object::ptr_type node_ptr_type;

			typedef oosl::type::id type_id_type;
			typedef oosl::type::object type_object_type;
			typedef type_object_type::ptr_type type_ptr_type;

			typedef oosl::storage::object::entry_type entry_type;

			explicit typed_grammar(const std::string &name = "OOSL_GENERIC_TYPED_GRAMMAR")
				: typed_grammar::base_type(start_, name){}

			template <typename parser_type>
			static auto keyword(parser_type parser){
				return boost::spirit::qi::lexeme[parser >> !boost::spirit::qi::char_("$_A-Za-z0-9")];
			};

			static auto keyword(const char *value){
				return keyword(boost::spirit::qi::lit(value));
			}

			template <typename parser_type>
			static auto operator_(parser_type parser){
				return boost::spirit::qi::lexeme[parser >> !(boost::spirit::qi::punct - boost::spirit::qi::char_(OOSL_AST_OPERATOR_EXCLUDES))];
			};

			static auto operator_(const char *value){
				return operator_(boost::spirit::qi::lit(value));
			}

		protected:
			rule_type start_;
		};

		class grammar : public typed_grammar<oosl::node::object::ptr_type>{
		public:
			typedef typed_grammar<oosl::node::object::ptr_type> base_type;
			typedef base_type::node_ptr_type node_ptr_type;

			explicit grammar(const std::string &name = "OOSL_GENERIC_GRAMMAR");
		};
	}
}

#endif /* !OOSL_GRAMMAR_H */
