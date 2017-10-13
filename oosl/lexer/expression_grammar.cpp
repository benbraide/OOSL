#include "expression_grammar.h"

oosl::lexer::range_grammar::range_grammar()
	: grammar("OOSL_RANGE"){
	using namespace boost::spirit;

	start_ = (integer_literal_ >> "..." >> integer_literal_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::range_grammar::create(node_ptr_type left, node_ptr_type right){
	typedef oosl::node::inplace<std::pair<node_ptr_type, node_ptr_type>> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::range, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = eval(owner.value().first, owner.value().second);
			return true;
		case inplace_target_type::print:
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write("...");
			owner.value().second->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::count:
			*reinterpret_cast<std::size_t *>(out) = count(owner.value().first, owner.value().second);
			return true;
		default:
			break;
		}

		return false;
	}, std::make_pair(left, right));
}

oosl::lexer::grammar::entry_type *oosl::lexer::range_grammar::eval(node_ptr_type left, node_ptr_type right){
	typedef oosl::node::numeric_inplace_value numeric_inplace_value_type;

	auto numeric_from = dynamic_cast<numeric_inplace_value_type *>(left.get());
	auto numeric_to = dynamic_cast<numeric_inplace_value_type *>(right.get());

	auto from_id = numeric_from->type_id();
	auto to_id = numeric_to->type_id();

	switch ((from_id < to_id) ? to_id : from_id){
	case type_id_type::int8_:
		return eval(numeric_from->converted_value<__int8>(), numeric_to->converted_value<__int8>());
	case type_id_type::uint8_:
		return eval(numeric_from->converted_value<unsigned __int8>(), numeric_to->converted_value<unsigned __int8>());
	case type_id_type::int16_:
		return eval(numeric_from->converted_value<__int16>(), numeric_to->converted_value<__int16>());
	case type_id_type::uint16_:
		return eval(numeric_from->converted_value<unsigned __int16>(), numeric_to->converted_value<unsigned __int16>());
	case type_id_type::int32_:
		return eval(numeric_from->converted_value<__int32>(), numeric_to->converted_value<__int32>());
	case type_id_type::uint32_:
		return eval(numeric_from->converted_value<unsigned __int32>(), numeric_to->converted_value<unsigned __int32>());
	case type_id_type::int64_:
		return eval(numeric_from->converted_value<__int64>(), numeric_to->converted_value<__int64>());
	case type_id_type::uint64_:
		return eval(numeric_from->converted_value<unsigned __int64>(), numeric_to->converted_value<unsigned __int64>());
	default:
		break;
	}

	throw oosl::common::error_codes::bad_range;
}

std::size_t oosl::lexer::range_grammar::count(node_ptr_type left, node_ptr_type right){
	typedef oosl::node::numeric_inplace_value numeric_inplace_value_type;

	auto numeric_from = dynamic_cast<numeric_inplace_value_type *>(left.get());
	auto numeric_to = dynamic_cast<numeric_inplace_value_type *>(right.get());

	auto from_id = numeric_from->type_id();
	auto to_id = numeric_to->type_id();

	switch ((from_id < to_id) ? to_id : from_id){
	case type_id_type::int8_:
		return count(numeric_from->converted_value<__int8>(), numeric_to->converted_value<__int8>());
	case type_id_type::uint8_:
		return count(numeric_from->converted_value<unsigned __int8>(), numeric_to->converted_value<unsigned __int8>());
	case type_id_type::int16_:
		return count(numeric_from->converted_value<__int16>(), numeric_to->converted_value<__int16>());
	case type_id_type::uint16_:
		return count(numeric_from->converted_value<unsigned __int16>(), numeric_to->converted_value<unsigned __int16>());
	case type_id_type::int32_:
		return count(numeric_from->converted_value<__int32>(), numeric_to->converted_value<__int32>());
	case type_id_type::uint32_:
		return count(numeric_from->converted_value<unsigned __int32>(), numeric_to->converted_value<unsigned __int32>());
	case type_id_type::int64_:
		return count(numeric_from->converted_value<__int64>(), numeric_to->converted_value<__int64>());
	case type_id_type::uint64_:
		return count(numeric_from->converted_value<unsigned __int64>(), numeric_to->converted_value<unsigned __int64>());
	default:
		break;
	}

	throw oosl::common::error_codes::bad_range;
}

oosl::lexer::non_operator_term_grammar::non_operator_term_grammar()
	: grammar("OOSL_NON_OPERATOR_TERM"){
	using namespace boost::spirit;

	grouped_expression_ = std::make_shared<grouped_expression_grammar>();
	list_expression_ = std::make_shared<list_expression_grammar>();
	static_cast_expression_ = std::make_shared<cast_expression_grammar<false>>();
	reinterpret_cast_expression_ = std::make_shared<cast_expression_grammar<true>>();

	start_ %= (literal_ | constant_ | system_call_ | type_ | (*static_cast_expression_) | (*reinterpret_cast_expression_) | identifier_compatible_ | (*grouped_expression_) | (*list_expression_));
}

oosl::lexer::right_unary_term_grammar::right_unary_term_grammar()
	: grammar("OOSL_RIGHT_UNARY_TERM"){
	using namespace boost::spirit;

	expression_ = std::make_shared<expression_grammar>();

	member_access_ = (member_access_operator_ >> identifier_compatible_)[qi::_val = boost::phoenix::bind(&create_member_access, qi::_1, qi::_2)];
	call_ = ('(' >> -expression_list_ >> ')')[qi::_val = boost::phoenix::bind(&create_call, qi::_1)];
	index_ = ('[' >> -(*expression_) >> ']')[qi::_val = boost::phoenix::bind(&create_index, qi::_1)];

	start_ = (
		non_operator_term_[qi::_val = qi::_1]
		>> *(member_access_ | call_ | index_)[qi::_val = boost::phoenix::bind(&create, qi::_val, qi::_1)]
		);
}

oosl::lexer::right_unary_term_grammar::member_access_info oosl::lexer::right_unary_term_grammar::create_member_access(const operator_info &info, node_ptr_type value){
	return member_access_info{ info, value };
}

oosl::lexer::right_unary_term_grammar::member_access_info oosl::lexer::right_unary_term_grammar::create_call(boost::optional<node_ptr_type> value){
	//value.value_or(nullptr);
	return member_access_info{ operator_info{ operator_id_type::index }, nullptr };
}

oosl::lexer::right_unary_term_grammar::member_access_info oosl::lexer::right_unary_term_grammar::create_index(boost::optional<node_ptr_type> value){
	return member_access_info{ operator_info{ operator_id_type::index }, value.value_or(nullptr) };
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::right_unary_term_grammar::create(node_ptr_type left, const member_access_info &right){
	return nullptr;
}

oosl::lexer::term_grammar::term_grammar()
	: grammar("OOSL_TERM"){
	using namespace boost::spirit;

	base_ = ((known_operator_ | unknown_operator_) >> right_unary_term_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];

	start_ %= (right_unary_term_ | base_);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::term_grammar::create(const operator_info &info, node_ptr_type value){
	return std::make_shared<oosl::node::unary_expression>(info, value);
}

oosl::lexer::expression_helper::node_ptr_type oosl::lexer::expression_helper::create(node_ptr_type left, const expression_extension &ext){
	return std::make_shared<oosl::node::binary_expression>(ext.info, left, ext.value);
}

oosl::lexer::term_grammar oosl::lexer::expression_helper::term_;

oosl::lexer::right_unary_term_grammar oosl::lexer::expression_helper::right_unary_term_;

oosl::lexer::non_list_expression_grammar::non_list_expression_grammar()
	: grammar("OOSL_NON_LIST_EXPRESSION"){
	ternary_expression_ = std::make_shared<ternary_expression_grammar_type>();

	start_ %= expression_helper::parser(*ternary_expression_);
}

oosl::lexer::expression_grammar::expression_grammar()
	: grammar("OOSL_EXPRESSION"){
	ternary_expression_ = std::make_shared<ternary_expression_grammar_type>();

	start_ %= expression_helper::parser(*ternary_expression_);
}

oosl::lexer::grouped_expression_grammar::grouped_expression_grammar()
	: grammar("OOSL_GROUPED_EXPRESSION"){
	using namespace boost::spirit;

	start_ = ('(' >> expression_ >> ')')[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::grouped_expression_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::range, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = owner.value()->evaluate();
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("(");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write(")");
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::list_expression_grammar::list_expression_grammar()
	: grammar("OOSL_LIST_EXPRESSION"){
	using namespace boost::spirit;

	start_ = ('[' >> -expression_list_ >> ']')[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::list_expression_grammar::create(boost::optional<node_ptr_type> value){
	return nullptr;
}

oosl::lexer::variadic_expression_grammar::variadic_expression_grammar()
	: grammar("OOSL_VARIADIC_EXPRESSION"){
	using namespace boost::spirit;

	start_ = (identifier_or_placeholder_ >> "...")[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::variadic_expression_grammar::create(node_ptr_type value){
	return nullptr;
}
