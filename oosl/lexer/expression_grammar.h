#pragma once

#ifndef OOSL_EXPRESSION_GRAMMAR_H
#define OOSL_EXPRESSION_GRAMMAR_H

#include "constant_grammar.h"
#include "general_grammar.h"
#include "type_grammar.h"
#include "operator_grammar.h"

#include "../node/expression_node.h"

namespace oosl{
	namespace lexer{
		class range_grammar : public grammar{
		public:
			range_grammar();

			static node_ptr_type create(node_ptr_type left, node_ptr_type right);

			static entry_type *eval(node_ptr_type left, node_ptr_type right);

			template <typename value_type>
			static entry_type *eval(value_type from, value_type to){
				return nullptr;
			}

			static std::size_t count(node_ptr_type left, node_ptr_type right);

			template <typename value_type>
			static std::size_t count(value_type from, value_type to){
				return ((from < to) ? ((to - from) + 1) : ((from - to) + 1));
			}

		protected:
			integer_literal_grammar integer_literal_;
		};

		class grouped_expression_grammar;
		class list_expression_grammar;
		class non_list_expression_grammar;

		template <bool reinterpret>
		class cast_expression_grammar;

		class non_operator_term_grammar : public grammar{
		public:
			typedef std::shared_ptr<grouped_expression_grammar> grouped_expression_grammar_type;
			typedef std::shared_ptr<list_expression_grammar> list_expression_grammar_type;

			typedef std::shared_ptr<cast_expression_grammar<false>> static_cast_expression_grammar_type;
			typedef std::shared_ptr<cast_expression_grammar<true>> reinterpret_cast_expression_grammar_type;

			non_operator_term_grammar();

		protected:
			literal_grammar literal_;
			constant_grammar constant_;
			identifier_compatible_grammar identifier_compatible_;
			system_call_grammar system_call_;
			grouped_expression_grammar_type grouped_expression_;
			list_expression_grammar_type list_expression_;
			static_cast_expression_grammar_type static_cast_expression_;
			reinterpret_cast_expression_grammar_type reinterpret_cast_expression_;
			type_grammar type_;
		};

		class right_unary_term_grammar : public grammar{
		public:
			typedef oosl::common::operator_info operator_info;
			typedef oosl::common::operator_id operator_id_type;
			typedef std::shared_ptr<expression_grammar> expression_grammar_type;

			struct member_access_info{
				operator_info info;
				node_ptr_type value;
			};

			typedef boost::spirit::qi::rule<iterator_type, member_access_info(), skipper> custom_rule_type;

			right_unary_term_grammar();

			static member_access_info create_member_access(const operator_info &info, node_ptr_type value);

			static member_access_info create_call(boost::optional<node_ptr_type> value);

			static member_access_info create_index(boost::optional<node_ptr_type> value);

			static node_ptr_type create(node_ptr_type left, const member_access_info &right);

		protected:
			custom_rule_type member_access_;
			custom_rule_type call_;
			custom_rule_type index_;
			non_operator_term_grammar non_operator_term_;
			member_access_operator_grammar member_access_operator_;
			identifier_compatible_grammar identifier_compatible_;
			expression_grammar_type expression_;
			expression_list_grammar expression_list_;
		};

		class term_grammar : public grammar{
		public:
			typedef oosl::common::operator_info operator_info;

			term_grammar();

			static node_ptr_type create(const operator_info &info, node_ptr_type value);

		protected:
			rule_type base_;
			right_unary_term_grammar right_unary_term_;
			known_operator_grammar known_operator_;
			unknown_operator_grammar unknown_operator_;
		};

		struct expression_extension{
			oosl::common::operator_info info;
			grammar::node_ptr_type value;
		};

		class expression_helper{
		public:
			typedef grammar::node_ptr_type node_ptr_type;

			template <typename target_type>
			static auto parser(target_type &target){
				return
					(right_unary_term_ | term_)[boost::spirit::qi::_val = boost::spirit::qi::_1]
					>> *target[boost::spirit::qi::_val = boost::phoenix::bind(&create, boost::spirit::qi::_val, boost::spirit::qi::_1)];
			}

			static node_ptr_type create(node_ptr_type left, const expression_extension &ext);

		private:
			static term_grammar term_;
			static right_unary_term_grammar right_unary_term_;
		};

		template <class operator_type, bool right_associative, class higher_type>
		class generic_expression_grammar : public typed_grammar<expression_extension>{
		public:
			typedef typed_grammar<expression_extension> base_type;
			typedef base_type::node_ptr_type node_ptr_type;

			explicit generic_expression_grammar(const std::string &name = "OOSL_GENERIC_EXPRESSION_GRAMMAR")
				: base_type(name){
				if (right_associative)
					ext_ = expression_helper::parser(*this);
				else//Left associative
					ext_ = expression_helper::parser(higher_);

				start_ %= (higher_ | (operator_ >> ext_));
			}

		protected:
			oosl::lexer::grammar::rule_type ext_;
			operator_type operator_;
			higher_type higher_;
		};

		template <class operator_type, bool right_associative>
		class generic_expression_grammar<operator_type, right_associative, void> : public typed_grammar<expression_extension>{
		public:
			typedef typed_grammar<expression_extension> base_type;
			typedef base_type::node_ptr_type node_ptr_type;

			explicit generic_expression_grammar(const std::string &name = "OOSL_GENERIC_EXPRESSION_GRAMMAR")
				: base_type(name){
				start_ %= (operator_ >> (right_unary_term_ | term_));
			}

		protected:
			operator_type operator_;
			term_grammar term_;
			right_unary_term_grammar right_unary_term_;
		};

		using factor_expression_grammar				= generic_expression_grammar<factor_operator_grammar, false, void>;
		using addition_expression_grammar			= generic_expression_grammar<addition_operator_grammar, false, factor_expression_grammar>;
		using shift_expression_grammar				= generic_expression_grammar<shift_operator_grammar, false, addition_expression_grammar>;
		using relation_expression_grammar			= generic_expression_grammar<relation_operator_grammar, false, shift_expression_grammar>;
		using equality_expression_grammar			= generic_expression_grammar<equality_operator_grammar, false, relation_expression_grammar>;
		using bitwise_and_expression_grammar		= generic_expression_grammar<bitwise_and_operator_grammar, false, equality_expression_grammar>;
		using bitwise_xor_expression_grammar		= generic_expression_grammar<bitwise_xor_operator_grammar, false, bitwise_and_expression_grammar>;
		using bitwise_or_expression_grammar			= generic_expression_grammar<bitwise_or_operator_grammar, false, bitwise_xor_expression_grammar>;
		using and_expression_grammar				= generic_expression_grammar<and_operator_grammar, false, bitwise_or_expression_grammar>;
		using or_expression_grammar					= generic_expression_grammar<or_operator_grammar, false, and_expression_grammar>;
		using unknown_expression_grammar			= generic_expression_grammar<unknown_operator_grammar, false, or_expression_grammar>;

		template <bool non_list>
		class ternary_expression_grammar;

		class non_list_expression_grammar : public grammar{
		public:
			typedef ternary_expression_grammar<true> ternary_expression_grammar_type;
			typedef std::shared_ptr<ternary_expression_grammar_type> ternary_expression_grammar_ptr_type;

			non_list_expression_grammar();

		protected:
			ternary_expression_grammar_ptr_type ternary_expression_;
		};

		class expression_grammar : public grammar{
		public:
			typedef ternary_expression_grammar<false> ternary_expression_grammar_type;
			typedef std::shared_ptr<ternary_expression_grammar_type> ternary_expression_grammar_ptr_type;

			expression_grammar();

		protected:
			ternary_expression_grammar_ptr_type ternary_expression_;
		};

		template <bool reinterpret>
		class cast_expression_grammar : public grammar{
		public:
			cast_expression_grammar()
				: grammar("OOSL_CAST_EXPRESSION_GRAMMAR"){
				using namespace boost::spirit;

				start_ = (qi::lit(reinterpret ? "reinterpret_cast" : "static_cast") >> '<' >> (type_ | ref_type_ | identifier_compatible_) >> qi::lit('>') >> '(' >> expression_ >> ')')
					[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
			}

			static node_ptr_type create(node_ptr_type type, node_ptr_type value){
				return nullptr;
			}

		protected:
			expression_grammar expression_;
			type_grammar type_;
			ref_type_grammar ref_type_;
			identifier_compatible_grammar identifier_compatible_;
		};

		template <bool non_list>
		class assignment_expression_grammar : public typed_grammar<expression_extension>{
		public:
			typedef typed_grammar<expression_extension> base_type;
			typedef base_type::node_ptr_type node_ptr_type;

			typedef boost::spirit::qi::rule<iterator_type, oosl::common::operator_info(), skipper> custom_rule_type;

			assignment_expression_grammar()
				: base_type("OOSL_ASSIGNMENT_EXPRESSION_GRAMMAR"){
				using namespace boost::spirit;
				if (non_list)
					operator_ %= assignment_operator_;
				else//Include comma expressions
					operator_ %= comma_operator_;

				if (non_list)
					right_ %= non_list_expression_;
				else//Include comma expressions
					right_ %= expression_;

				start_ %= (higher_ | (operator_ >> right_));
			}

		protected:
			custom_rule_type operator_;
			oosl::lexer::grammar::rule_type right_;
			unknown_expression_grammar higher_;
			assignment_operator_grammar assignment_operator_;
			comma_operator_grammar comma_operator_;
			expression_grammar expression_;
			non_list_expression_grammar non_list_expression_;
		};

		template <bool non_list>
		class ternary_expression_grammar : public typed_grammar<expression_extension>{
		public:
			typedef typed_grammar<expression_extension> base_type;
			typedef base_type::node_ptr_type node_ptr_type;

			ternary_expression_grammar()
				: base_type("OOSL_TERNARY_EXPRESSION_GRAMMAR"){
				using namespace boost::spirit;
				if (non_list)
					right_ %= non_list_expression_;
				else//Include comma expressions
					right_ %= expression_;

				base_ = (operator_ >> expression_ >> ":" >> right_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3)];

				start_ %= (assignment_expression_ | base_);
			}

			static expression_extension create(oosl::common::operator_info info, node_ptr_type left, node_ptr_type right){
				return expression_extension{ info };
			}

		protected:
			rule_type base_;
			oosl::lexer::grammar::rule_type right_;
			ternary_operator_grammar operator_;
			expression_grammar expression_;
			non_list_expression_grammar non_list_expression_;
			assignment_expression_grammar<non_list> assignment_expression_;
		};

		class grouped_expression_grammar : public grammar{
		public:
			grouped_expression_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			expression_grammar expression_;
		};

		class list_expression_grammar : public grammar{
		public:
			list_expression_grammar();

			static node_ptr_type create(boost::optional<node_ptr_type> value);

		protected:
			expression_list_grammar expression_list_;
		};

		class variadic_expression_grammar : public grammar{
		public:
			variadic_expression_grammar();

			static node_ptr_type create(node_ptr_type value);

		protected:
			identifier_or_placeholder_grammar identifier_or_placeholder_;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	oosl::lexer::expression_extension,
	(oosl::common::operator_info, info)
	(oosl::lexer::grammar::node_ptr_type, value)
)

#endif /* !OOSL_EXPRESSION_GRAMMAR_H */
