#pragma once

#ifndef OOSL_OPERATOR_GRAMMAR_H
#define OOSL_OPERATOR_GRAMMAR_H

#include "operator_ast.h"

#define OOSL_AST_OPERATOR_EXCLUDES "()[]{}\"'`\\,;:$_"

namespace oosl{
	namespace lexer{
		template <class ast_type, oosl::common::operator_id id>
		class operator_grammar : public boost::spirit::qi::grammar<const char *, ast_type(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef oosl::common::operator_id operator_id_type;

			typedef boost::spirit::qi::rule<iterator_type, ast_type(), skipper> start_rule_type;
			typedef boost::spirit::qi::symbols<char, operator_id_type> operator_id_symbols_type;

			operator_grammar()
				: operator_grammar::base_type(start_, "OOSL_OPERATOR"){
				using namespace boost::spirit;
				start_ %= symbols_ >> qi::omit[qi::no_skip[!(qi::punct - qi::char_(OOSL_AST_OPERATOR_EXCLUDES))]];

				switch (id){
				case operator_id_type::member_access:
					symbols_.add
						("->", operator_id_type::member_pointer_access)
						(".", operator_id_type::member_access);
					break;
				case operator_id_type::times:
					symbols_.add
						("*", operator_id_type::times)
						("/", operator_id_type::divide)
						("%", operator_id_type::modulus);
					break;
				case operator_id_type::plus:
					symbols_.add
						("+", operator_id_type::plus)
						("-", operator_id_type::minus);
					break;
				case operator_id_type::left_shift:
					symbols_.add
						("<<", operator_id_type::left_shift)
						(">>", operator_id_type::right_shift);
					break;
				case operator_id_type::less:
					symbols_.add
						("<=", operator_id_type::less_or_equal)
						(">=", operator_id_type::more_or_equal)
						("<", operator_id_type::less)
						(">", operator_id_type::more);
					break;
				case operator_id_type::equality:
					symbols_.add
						("===", operator_id_type::explicit_equality)
						("!==", operator_id_type::explicit_inverse_equality)
						("==", operator_id_type::equality)
						("!=", operator_id_type::inverse_equality);
					break;
				case operator_id_type::bitwise_and:
					symbols_.add
						("&", operator_id_type::bitwise_and);
					break;
				case operator_id_type::bitwise_xor:
					symbols_.add
						("^", operator_id_type::bitwise_xor);
					break;
				case operator_id_type::bitwise_or:
					symbols_.add
						("|", operator_id_type::bitwise_or);
					break;
				case operator_id_type::relational_and:
					symbols_.add
						("&&", operator_id_type::relational_and);
					break;
				case operator_id_type::relational_or:
					symbols_.add
						("||", operator_id_type::relational_or);
					break;
				case operator_id_type::comma:
					symbols_.add
					(",", operator_id_type::comma);
					break;
				case operator_id_type::ternary:
					symbols_.add
					("?", operator_id_type::ternary);
					break;
				case operator_id_type::assignment:
					symbols_.add
						("<<=", operator_id_type::compound_left_shift)
						(">>=", operator_id_type::compound_right_shift)
						("*=", operator_id_type::compound_times)
						("/=", operator_id_type::compound_divide)
						("%=", operator_id_type::compound_modulus)
						("+=", operator_id_type::compound_plus)
						("-=", operator_id_type::compound_minus)
						("&=", operator_id_type::compound_bitwise_and)
						("^=", operator_id_type::compound_bitwise_xor)
						("|=", operator_id_type::compound_bitwise_or)
						("=", operator_id_type::assignment);
					break;
				default:
					break;
				}
			}

		protected:
			start_rule_type start_;
			operator_id_symbols_type symbols_;
		};

		using member_access_operator_grammar	= operator_grammar<OOSL_AST_OPERATOR_NAME(member_access), oosl::common::operator_id::member_access>;
		using factor_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(factor), oosl::common::operator_id::times>;
		using addition_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(addition), oosl::common::operator_id::plus>;
		using shift_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(shift), oosl::common::operator_id::left_shift>;
		using relation_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(relation), oosl::common::operator_id::less>;
		using equality_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(equality), oosl::common::operator_id::equality>;
		using bitwise_and_operator_grammar		= operator_grammar<OOSL_AST_OPERATOR_NAME(bitwise_and), oosl::common::operator_id::bitwise_and>;
		using bitwise_xor_operator_grammar		= operator_grammar<OOSL_AST_OPERATOR_NAME(bitwise_xor), oosl::common::operator_id::bitwise_xor>;
		using bitwise_or_operator_grammar		= operator_grammar<OOSL_AST_OPERATOR_NAME(bitwise_or), oosl::common::operator_id::bitwise_or>;
		using and_operator_grammar				= operator_grammar<OOSL_AST_OPERATOR_NAME(and), oosl::common::operator_id::relational_and>;
		using or_operator_grammar				= operator_grammar<OOSL_AST_OPERATOR_NAME(or), oosl::common::operator_id::relational_or>;
		using comma_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(comma), oosl::common::operator_id::comma>;
		using ternary_operator_grammar			= operator_grammar<OOSL_AST_OPERATOR_NAME(ternary), oosl::common::operator_id::ternary>;
		using assignment_operator_grammar		= operator_grammar<OOSL_AST_OPERATOR_NAME(assignment), oosl::common::operator_id::assignment>;

		class unknown_operator_grammar : public boost::spirit::qi::grammar<const char *, OOSL_AST_OPERATOR_NAME(unknown)(), skipper>{
		public:
			typedef const char *iterator_type;
			typedef boost::spirit::qi::rule<iterator_type, OOSL_AST_OPERATOR_NAME(unknown)(), skipper> start_rule_type;

			unknown_operator_grammar();

		protected:
			start_rule_type start_;
		};
	}
}

#endif /* !OOSL_OPERATOR_GRAMMAR_H */
