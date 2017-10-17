#pragma once

#ifndef OOSL_GENERAL_STATEMENT_GRAMMAR_H
#define OOSL_GENERAL_STATEMENT_GRAMMAR_H

#include "expression_grammar.h"

namespace oosl{
	namespace lexer{
		template <oosl::common::error_codes code>
		class break_or_continue_statement_grammar : public grammar{
		public:
			break_or_continue_statement_grammar()
				: grammar("OOSL_BREAK_OR_CONTINUE_STATEMENT"){
				using namespace boost::spirit;

				start_ = keyword((code == oosl::common::error_codes::break_) ? "break" : "continue")[qi::_val = create()];
			}

			static node_ptr_type create(){
				typedef oosl::node::inplace<void> node_type;

				auto id = ((code == oosl::common::error_codes::break_) ? node_id_type::break_ : node_id_type::continue_);
				return std::make_shared<node_type>(id, [](node_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						throw code;
						return true;
					case inplace_target_type::print:
						reinterpret_cast<output_writer_type *>(out)->write((code == oosl::common::error_codes::break_) ? "break" : "continue");
						return true;
					default:
						break;
					}

					return false;
				});
			}
		};

		class return_statement_grammar : public grammar{
		public:
			return_statement_grammar();

			static node_ptr_type create(boost::optional<node_ptr_type> value);

		protected:
			expression_grammar expression_;
		};

		class throw_statement_grammar : public grammar{
		public:
			throw_statement_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar expression_;
		};

		class rethrow_statement_grammar : public grammar{
		public:
			rethrow_statement_grammar();

			static node_ptr_type create();
		};

		class case_statement_grammar : public grammar{
		public:
			case_statement_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar expression_;
		};

		class default_statement_grammar : public grammar{
		public:
			default_statement_grammar();

			static node_ptr_type create();
		};

		class echo_statement_grammar : public grammar{
		public:
			echo_statement_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar expression_;
		};
	}
}

#endif /* !OOSL_GENERAL_STATEMENT_GRAMMAR_H */
