#include "string_driver.h"

oosl::driver::string_driver::~string_driver() = default;

oosl::driver::object::entry_type *oosl::driver::string_driver::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	if (!OOSL_IS_ANY(options, cast_option_type::reinterpret | cast_option_type::ref) && !type.is_ref()){
		switch (type.id()){
		case type_id_type::string_:
			return common::controller::active->temporary_storage().add_scalar(value<std::string>(entry));
		case type_id_type::wstring_:
			return common::controller::active->temporary_storage().add_scalar(value<std::wstring>(entry));
		default:
			break;
		}
	}

	return object::cast(entry, type, options);
}

void oosl::driver::string_driver::echo(entry_type &entry, output_writer_type &writer){
	switch (entry.type->id()){
	case type_id_type::string_:
		return echo_<std::string>(entry, writer);
	case type_id_type::wstring_:
		return echo_<std::wstring>(entry, writer);
	default:
		break;
	}

	throw error_type::not_implemented;
}

void oosl::driver::string_driver::value(entry_type &entry, type_id_type to, char *destination){
	if (to == entry.type->id()){
		switch (entry.type->id()){
		case type_id_type::string_:
			return value_from_<std::string>(entry, to, destination);
		case type_id_type::wstring_:
			return value_from_<std::wstring>(entry, to, destination);
		default:
			break;
		}
	}

	throw error_type::not_implemented;
}

oosl::driver::object::entry_type *oosl::driver::string_driver::evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand){
	try{//Guard
		switch (entry.type->id()){
		case type_id_type::string_:
			return evaluate_<std::string>(entry, operator_info.id, operand, type_id_type::string_);
		case type_id_type::wstring_:
			return evaluate_<std::wstring>(entry, operator_info.id, operand, type_id_type::wstring_);
		default:
			break;
		}
	}
	catch (error_type error){//Error
		if (error == error_type::not_implemented)
			throw error_type::unhandled_operator;
		throw;//Forward exception
	}

	return object::evaluate_(entry, operator_info, operand);
}
