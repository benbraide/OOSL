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

	start_ %= (literal_ | constant_ | system_call_ | type_ | identifier_compatible_ | (*grouped_expression_) | (*list_expression_));
}

oosl::lexer::expression_grammar::expression_grammar()
	: grammar("OOSL_EXPRESSION"){

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

}
