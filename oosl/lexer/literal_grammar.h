#pragma once

#ifndef OOSL_LITERAL_GRAMMAR_H
#define OOSL_LITERAL_GRAMMAR_H

#include <climits>

#include "grammar.h"

namespace oosl{
	namespace lexer{
		enum class integral_suffix_type{
			nil,
			i8,
			ui8,
			i16,
			ui16,
			i32,
			ui32,
			i64,
			ui64,
			i128,
			ui128,
		};

		enum class real_suffix_type{
			nil,
			f32,
			f64,
			f128,
		};

		enum class quote_prefix_type{
			nil,
			wide,
		};

		struct numeric_helper{
			typedef oosl::node::id node_id_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::node::object::ptr_type node_ptr_type;

			typedef oosl::storage::object::entry_type entry_type;
			typedef oosl::common::output_writer output_writer_type;

			static bool pass_radix(int value);

			template <typename node_type, typename value_type>
			static node_ptr_type make_node(value_type value, const std::string &suffix){
				typedef typename node_type::value_type target_value_type;
				if (sizeof(value_type) > sizeof(target_value_type) && value > static_cast<value_type>(std::numeric_limits<target_value_type>::max()))
					throw oosl::common::error_codes::number_too_big;

				return std::make_shared<node_type>(node_id_type::literal_, [=](inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(static_cast<target_value_type>(value));
						return true;
					case inplace_target_type::print:
						if (std::is_floating_point_v<target_value_type>)
							reinterpret_cast<output_writer_type *>(out)->write(oosl::common::controller::real_to_string<std::string>(static_cast<target_value_type>(value)) + suffix);
						else//Integral value
							reinterpret_cast<output_writer_type *>(out)->write(std::to_string(static_cast<target_value_type>(value)) + suffix);
						return true;
					default:
						break;
					}

					return false;
				}, static_cast<target_value_type>(value));
			}

			static node_ptr_type make_rad_node(int base, const std::string &value, boost::optional<integral_suffix_type> suffix);

			static node_ptr_type make_int_node(long long value, boost::optional<integral_suffix_type> suffix);

			static node_ptr_type make_real_node(long double value, boost::optional<real_suffix_type> suffix);
		};

		class rad_literal_grammar : public grammar{
		public:
			typedef boost::spirit::qi::symbols<char, integral_suffix_type> suffix_symbols_type;

			rad_literal_grammar();

		protected:
			suffix_symbols_type suffix_symbols_;
		};

		class integer_literal_grammar : public grammar{
		public:
			typedef boost::spirit::qi::symbols<char, integral_suffix_type> suffix_symbols_type;

			integer_literal_grammar();

		protected:
			suffix_symbols_type suffix_symbols_;
		};

		class real_literal_grammar : public grammar{
		public:
			typedef const char *iterator_type;

			template <typename value_type>
			struct real_policies : boost::spirit::qi::real_policies<value_type>{
				static bool const expect_dot = true;
				static bool const allow_leading_dot = true;
				static bool const allow_trailing_dot = true;
			};

			typedef boost::spirit::qi::real_parser<long double, real_policies<long double>> real_parser_type;
			typedef boost::spirit::qi::symbols<char, real_suffix_type> suffix_symbols_type;

			real_literal_grammar();

		protected:
			real_parser_type real_parser_;
			suffix_symbols_type suffix_symbols_;
		};

		class numeric_literal_grammar : public grammar{
		public:
			numeric_literal_grammar();

		protected:
			rad_literal_grammar rad_;
			integer_literal_grammar integer_;
			real_literal_grammar real_;
		};

		template <bool is_string>
		class basic_character_literal_grammar : public grammar{
		public:
			typedef boost::spirit::qi::uint_parser<char, 16, 2, 2> hex2_type;
			typedef boost::spirit::qi::uint_parser<wchar_t, 16, 4, 4> hex4_type;

			typedef boost::spirit::qi::symbols<char, char> escape_symbols_type;

			basic_character_literal_grammar()
				: grammar(is_string ? "OOSL_STRING_LITERAL" : "OOSL_CHARACTER_LITERAL"){
				using namespace boost::spirit;

				if (is_string){//Sequence
					escaped_ = qi::as_string[qi::lexeme['\"' > *(escape_symbols_ | ("\\x" > hex2_) | ~qi::char_('\"')) > '\"']][qi::_val = boost::phoenix::bind(&create_escaped_string, qi::_1)];
					non_escaped_ = qi::as_string[qi::lexeme["@'" > *(~qi::char_('\'')) > '\'']][qi::_val = boost::phoenix::bind(&create_non_escaped_string, qi::_1)];

					escaped_wide_ = qi::as_wstring[qi::lexeme["L\"" > *(escape_symbols_ | ("\\x" > hex4_) | ~qi::char_('\"')) > '\"']][qi::_val = boost::phoenix::bind(&create_escaped_wstring, qi::_1)];
					non_escaped_wide_ = qi::as_wstring[qi::lexeme["@L\"" > *(~qi::char_('\"')) > '\"']][qi::_val = boost::phoenix::bind(&create_non_escaped_wstring, qi::_1)];
				}
				else{//Single
					escaped_ = qi::as_string[qi::lexeme['\'' > -(escape_symbols_ | ("\\x" > hex2_) | ~qi::char_('\'')) > '\'']][qi::_val = boost::phoenix::bind(&create_escaped_string, qi::_1)];
					non_escaped_ = qi::as_string[qi::lexeme["@'" > -(~qi::char_('\'')) > '\'']][qi::_val = boost::phoenix::bind(&create_non_escaped_string, qi::_1)];

					escaped_wide_ = qi::as_wstring[qi::lexeme["L'" > -(escape_symbols_ | ("\\x" > hex4_) | ~qi::char_('\'')) > '\'']][qi::_val = boost::phoenix::bind(&create_escaped_wstring, qi::_1)];
					non_escaped_wide_ = qi::as_wstring[qi::lexeme["@L'" > -(~qi::char_('\'')) > '\'']][qi::_val = boost::phoenix::bind(&create_non_escaped_wstring, qi::_1)];
				}

				start_ %= (escaped_ | escaped_wide_ | non_escaped_ | non_escaped_wide_);

				escape_symbols_.add
					("\\0", '\0')
					("\\a", '\a')
					("\\b", '\b')
					("\\f", '\f')
					("\\n", '\n')
					("\\r", '\r')
					("\\t", '\t')
					("\\v", '\v')
					("\\?", '\?')
					("\\\"", '\"')
					("\\\'", '\'')
					("\\\\", '\\');
			}

			static node_ptr_type create_escaped_string(const std::string &value){
				return create(value, true);
			}

			static node_ptr_type create_escaped_wstring(const std::wstring &value){
				return create(value, true);
			}

			static node_ptr_type create_non_escaped_string(const std::string &value){
				return create(value, false);
			}

			static node_ptr_type create_non_escaped_wstring(const std::wstring &value){
				return create(value, false);
			}

			template <typename string_type>
			static node_ptr_type create(const string_type &value, bool escaped){
				typedef oosl::node::inplace<string_type> node_type;
				typedef typename string_type::traits_type::char_type char_type;

				/*string_type escaped_value;
				if (escaped && !value.empty())//Escape string
					escape(value, escaped_value);*/

				return std::make_shared<node_type>(node_id_type::literal_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						if (is_string)
							*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(owner.value());
						else//char
							*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(get_char(owner.value()));
						return true;
					case inplace_target_type::print:
						if (is_string)
							reinterpret_cast<output_writer_type *>(out)->write(owner.value(), write_option_type::string_value);
						else//char
							reinterpret_cast<output_writer_type *>(out)->write(owner.value(), write_option_type::char_value);
						return true;
					default:
						break;
					}

					return false;
				}, value);
			}

			template <typename string_type>
			static typename string_type::traits_type::char_type get_char(const string_type &value){
				return (value.empty() ? static_cast<typename string_type::traits_type::char_type>('\0') : value[0]);
			}

			/*template <typename string_type>
			static void escape(const string_type &value, string_type &out){
				typedef typename string_type::traits_type::char_type char_type;

				out.reserve(value.size());
				for (std::size_t index = 0; index < value.size(); ++index){
					auto c = value[index];
					if (c == '\\')//Escape
						c = escape(c, value, index);
					out += c;//Append
				}
			}*/

		protected:
			hex2_type hex2_;
			hex4_type hex4_;

			rule_type escaped_;
			rule_type non_escaped_;

			rule_type escaped_wide_;
			rule_type non_escaped_wide_;

			escape_symbols_type escape_symbols_;
		};

		using character_literal_grammar = basic_character_literal_grammar<false>;
		using string_literal_grammar = basic_character_literal_grammar<true>;

		class expression_grammar;

		class raw_literal_grammar : public grammar{
		public:
			typedef std::shared_ptr<expression_grammar> expression_grammar_type;

			raw_literal_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar_type expression_;
		};

		class quote_literal_grammar : public grammar{
		public:
			quote_literal_grammar();

		protected:
			character_literal_grammar character_;
			string_literal_grammar string_;
			raw_literal_grammar raw_;
		};

		class literal_grammar : public grammar{
		public:
			literal_grammar();

		protected:
			numeric_literal_grammar numeric_;
			quote_literal_grammar quote_;
		};
	}
}

#endif /* !OOSL_LITERAL_GRAMMAR_H */
