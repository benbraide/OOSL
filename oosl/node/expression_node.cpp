#include "expression_node.h"

std::string oosl::node::expression_helper::known_operator_name(operator_id_type id){
	switch (id){
	case operator_id_type::compound_left_shift:
		return "<<=";
	case operator_id_type::compound_right_shift:
		return ">>=";
	case operator_id_type::compound_times:
		return "*=";
	case operator_id_type::compound_divide:
		return "/=";
	case operator_id_type::compound_modulus:
		return "%=";
	case operator_id_type::compound_plus:
		return "+=";
	case operator_id_type::compound_minus:
		return "-=";
	case operator_id_type::compound_bitwise_and:
		return "&=";
	case operator_id_type::compound_bitwise_xor:
		return "^=";
	case operator_id_type::compound_bitwise_or:
		return "|=";
	case operator_id_type::explicit_equality:
		return "===";
	case operator_id_type::explicit_inverse_equality:
		return "!==";
	case operator_id_type::equality:
		return "==";
	case operator_id_type::inverse_equality:
		return "!=";
	case operator_id_type::relational_and:
		return "&&";
	case operator_id_type::relational_or:
		return "||";
	case operator_id_type::member_pointer_access:
		return "->";
	case operator_id_type::left_shift:
		return "<<";
	case operator_id_type::right_shift:
		return ">>";
	case operator_id_type::less_or_equal:
		return "<=";
	case operator_id_type::more_or_equal:
		return ">=";
	case operator_id_type::increment:
		return "++";
	case operator_id_type::decrement:
		return "--";
	case operator_id_type::assignment:
		return "=";
	case operator_id_type::member_access:
		return ".";
	case operator_id_type::times:
		return "*";
	case operator_id_type::divide:
		return "/";
	case operator_id_type::modulus:
		return "%";
	case operator_id_type::plus:
		return "+";
	case operator_id_type::minus:
		return "-";
	case operator_id_type::less:
		return "<";
	case operator_id_type::more:
		return ">";
	case operator_id_type::bitwise_and:
		return "&";
	case operator_id_type::bitwise_xor:
		return "^";
	case operator_id_type::bitwise_or:
		return "|";
	case operator_id_type::bitwise_inverse:
		return "~";
	case operator_id_type::relational_not:
		return "!";
	case operator_id_type::comma:
		return ",";
	case operator_id_type::new_:
		return "new";
	case operator_id_type::delete_:
		return "delete";
	case operator_id_type::sizeof_:
		return "sizeof";
	case operator_id_type::typeof:
		return "typeof";
	default:
		break;
	}

	return "";
}

oosl::node::unary_expression::unary_expression(const operator_info_type &info, ptr_type operand)
	: info_(info), operand_(operand){}

oosl::node::unary_expression::~unary_expression() = default;

oosl::node::object::id_type oosl::node::unary_expression::id(){
	return id_type::nil;
}

void oosl::node::unary_expression::echo(output_writer_type &writer){
	writer.write(expression_helper::known_operator_name(info_.id));
	switch (info_.id){
	case operator_id_type::new_:
	case operator_id_type::delete_:
	case operator_id_type::sizeof_:
	case operator_id_type::typeof:
		writer.write(" ");
		break;
	default:
		break;
	}

	operand_->echo(writer);
}

oosl::node::object::entry_type *oosl::node::unary_expression::evaluate_(){
	return nullptr;
}

oosl::node::binary_expression::binary_expression(const operator_info_type &info, ptr_type left, ptr_type right)
	: info_(info), left_(left), right_(right){}

oosl::node::binary_expression::~binary_expression() = default;

oosl::node::object::id_type oosl::node::binary_expression::id(){
	return id_type::nil;
}

void oosl::node::binary_expression::echo(output_writer_type &writer){
	left_->echo(writer);
	if (info_.id != operator_id_type::comma)
		writer.write(" ");//Space before operator

	writer.write(expression_helper::known_operator_name(info_.id));
	writer.write(" ");//Space after operator

	right_->echo(writer);
}

oosl::node::object::entry_type *oosl::node::binary_expression::evaluate_(){
	return nullptr;
}
