#pragma once

#ifndef OOSL_OPERATOR_GRAMMAR_H
#define OOSL_OPERATOR_GRAMMAR_H

#include "grammar.h"

#include "../common/operator_info.h"

#define OOSL_AST_OPERATOR_EXCLUDES "()[]{}\"'`\\,;:$_"

namespace oosl{
	namespace lexer{
		template <oosl::common::operator_id id>
		class operator_grammar : public typed_grammar<oosl::common::operator_info>{
		public:
			typedef typed_grammar<oosl::common::operator_info> base_type;

			typedef oosl::common::operator_info operator_info;
			typedef oosl::common::operator_id operator_id_type;

			typedef boost::spirit::qi::symbols<char, operator_id_type> operator_id_symbols_type;

			operator_grammar()
				: base_type("OOSL_OPERATOR"){
				using namespace boost::spirit;

				if (id != operator_id_type::unknown)//Known
					start_ = symbols_[qi::_val = boost::phoenix::bind(&create_known, qi::_1)];
				else//Unknown
					start_ = qi::as_string[qi::lexeme[+(qi::punct - qi::char_(OOSL_AST_OPERATOR_EXCLUDES))]][qi::_val = boost::phoenix::bind(&create_unknown, qi::_1)];

				switch (id){
				case operator_id_type::nil:
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
						("===", operator_id_type::explicit_equality)
						("!==", operator_id_type::explicit_inverse_equality)
						("==", operator_id_type::equality)
						("!=", operator_id_type::inverse_equality)
						("&&", operator_id_type::relational_and)
						("||", operator_id_type::relational_or)
						("->", operator_id_type::member_pointer_access)
						("<<", operator_id_type::left_shift)
						(">>", operator_id_type::right_shift)
						("<=", operator_id_type::less_or_equal)
						(">=", operator_id_type::more_or_equal)
						("++", operator_id_type::increment)
						("--", operator_id_type::decrement)
						("=", operator_id_type::assignment)
						(".", operator_id_type::member_access)
						("*", operator_id_type::times)
						("/", operator_id_type::divide)
						("%", operator_id_type::modulus)
						("+", operator_id_type::plus)
						("-", operator_id_type::minus)
						("<", operator_id_type::less)
						(">", operator_id_type::more)
						("&", operator_id_type::bitwise_and)
						("^", operator_id_type::bitwise_xor)
						("|", operator_id_type::bitwise_or)
						("~", operator_id_type::bitwise_not)
						("!", operator_id_type::relational_not)
						(",", operator_id_type::comma)
						("new", operator_id_type::new_)
						("delete", operator_id_type::delete_)
						("sizeof", operator_id_type::sizeof_)
						("typeof", operator_id_type::typeof);
					break;
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

			static operator_info create_known(operator_id_type id){
				return operator_info{ id };
			}

			static operator_info create_unknown(const std::string &value){
				return operator_info{ operator_id_type::unknown, value };
			}

		protected:
			operator_id_symbols_type symbols_;
		};

		using member_access_operator_grammar	= operator_grammar<oosl::common::operator_id::member_access>;
		using factor_operator_grammar			= operator_grammar<oosl::common::operator_id::times>;
		using addition_operator_grammar			= operator_grammar<oosl::common::operator_id::plus>;
		using shift_operator_grammar			= operator_grammar<oosl::common::operator_id::left_shift>;
		using relation_operator_grammar			= operator_grammar<oosl::common::operator_id::less>;
		using equality_operator_grammar			= operator_grammar<oosl::common::operator_id::equality>;
		using bitwise_and_operator_grammar		= operator_grammar<oosl::common::operator_id::bitwise_and>;
		using bitwise_xor_operator_grammar		= operator_grammar<oosl::common::operator_id::bitwise_xor>;
		using bitwise_or_operator_grammar		= operator_grammar<oosl::common::operator_id::bitwise_or>;
		using and_operator_grammar				= operator_grammar<oosl::common::operator_id::relational_and>;
		using or_operator_grammar				= operator_grammar<oosl::common::operator_id::relational_or>;
		using comma_operator_grammar			= operator_grammar<oosl::common::operator_id::comma>;
		using ternary_operator_grammar			= operator_grammar<oosl::common::operator_id::ternary>;
		using assignment_operator_grammar		= operator_grammar<oosl::common::operator_id::assignment>;
		using known_operator_grammar			= operator_grammar<oosl::common::operator_id::nil>;
		using unknown_operator_grammar			= operator_grammar<oosl::common::operator_id::unknown>;
	}
}

#endif /* !OOSL_OPERATOR_GRAMMAR_H */
