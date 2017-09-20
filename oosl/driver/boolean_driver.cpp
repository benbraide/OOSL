#include "boolean_driver.h"

oosl::driver::boolean::~boolean() = default;

void oosl::driver::boolean::echo(entry_type &entry, output_writer_type &writer){
	switch (value<bool_type>(entry)){
	case bool_type::false_:
		writer.write("false");
		break;
	case bool_type::indeterminate:
		writer.write("indeterminate");
		break;
	case bool_type::true_:
		writer.write("true");
		break;
	default:
		throw error_type::not_implemented;
		break;
	}
}

void oosl::driver::boolean::value(entry_type &entry, type_id_type to, char *destination){
	if (to == type_id_type::bool_){
		auto value = oosl::common::controller::active->memory().read<bool_type>(entry.address);
		memcpy(destination, &value, sizeof(bool_type));
	}
}

oosl::driver::object::entry_type *oosl::driver::boolean::evaluate_(entry_type &entry, unary_operator_info_type &operator_info){
	if (operator_info.is_left && operator_info.id == operator_id_type::relational_not)
		return oosl::common::controller::active->temporary_storage().add_scalar((value<bool_type>(entry) == bool_type::false_) ? bool_type::true_ : bool_type::false_);
	return object::evaluate_(entry, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::boolean::evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand){
	return evaluate_equality_<bool_type>(entry, operator_info.id, operand);
}
