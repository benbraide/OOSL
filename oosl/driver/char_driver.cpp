#include "char_driver.h"

oosl::driver::char_driver::~char_driver() = default;

oosl::driver::object::entry_type *oosl::driver::char_driver::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	if (!OOSL_IS_ANY(options, cast_option_type::reinterpret | cast_option_type::ref) && !type.is_ref()){
		switch (type.id()){
		case type_id_type::char_:
			return common::controller::active->temporary_storage().add_scalar(value<char>(entry));
		case type_id_type::wchar_:
			return common::controller::active->temporary_storage().add_scalar(value<wchar_t>(entry));
		default:
			break;
		}
	}

	return object::cast(entry, type, options);
}

void oosl::driver::char_driver::echo(entry_type &entry, output_writer_type &writer){
	switch (entry.type->id()){
	case type_id_type::char_:
		return echo_<std::string>(entry, writer, type_id_type::string_);
	case type_id_type::wchar_:
		return echo_<std::wstring>(entry, writer, type_id_type::wstring_);
	default:
		break;
	}

	throw error_type::not_implemented;
}

void oosl::driver::char_driver::value(entry_type &entry, type_id_type to, char *destination){
	switch (entry.type->id()){
	case type_id_type::char_:
		return value_from_<char>(entry, to, destination);
	case type_id_type::wchar_:
		return value_from_<wchar_t>(entry, to, destination);
	default:
		break;
	}

	throw error_type::not_implemented;
}

oosl::driver::object::entry_type *oosl::driver::char_driver::evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand){
	auto operand_type = operand.type->driver()->type(operand);
	if (operand_type->id() != type_id_type::char_ && operand_type->id() != type_id_type::wchar_){//Non-numeric operand
		if (operator_info.id != operator_id_type::plus)//Not supported
			return object::evaluate_(entry, operator_info, operand);

		if (operand_type->is_string() && entry.type->id() == type_id_type::char_)
			return concatenate_string_<std::string>(entry, operand, type_id_type::string_);

		if (operand_type->is_wstring() && entry.type->id() == type_id_type::wchar_)
			return concatenate_string_<std::wstring>(entry, operand, type_id_type::wstring_);

		return object::evaluate_(entry, operator_info, operand);
	}

	if (operand_type->id() != entry.type->id())//Different types
		return object::evaluate_(entry, operator_info, operand);

	switch (entry.type->id()){
	case type_id_type::char_:
		return evaluate_<char, std::string>(entry, operator_info.id, operand, type_id_type::string_);
	case type_id_type::wchar_:
		return evaluate_<wchar_t, std::wstring>(entry, operator_info.id, operand, type_id_type::wstring_);
	default:
		break;
	}

	return object::evaluate_(entry, operator_info, operand);
}
