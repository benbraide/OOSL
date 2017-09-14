#include "numeric_driver.h"

oosl::driver::numeric::~numeric() = default;

oosl::driver::object::entry_type *oosl::driver::numeric::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	if (!OOSL_IS_ANY(options, cast_option_type::reinterpret | cast_option_type::ref) && !type.is_ref()){
		switch (entry.type->id()){
		case type_id_type::int8_:
			return common::controller::active->temporary_storage().add_scalar(value<__int8>(entry));
		case type_id_type::uint8_:
			return common::controller::active->temporary_storage().add_scalar(value<unsigned __int8>(entry));
		case type_id_type::int16_:
			return common::controller::active->temporary_storage().add_scalar(value<__int16>(entry));
		case type_id_type::uint16_:
			return common::controller::active->temporary_storage().add_scalar(value<unsigned __int16>(entry));
		case type_id_type::int32_:
			return common::controller::active->temporary_storage().add_scalar(value<__int32>(entry));
		case type_id_type::uint32_:
			return common::controller::active->temporary_storage().add_scalar(value<unsigned __int32>(entry));
		case type_id_type::int64_:
			return common::controller::active->temporary_storage().add_scalar(value<__int64>(entry));
		case type_id_type::uint64_:
			return common::controller::active->temporary_storage().add_scalar(value<unsigned __int64>(entry));
		/*case type_id_type::int128_:
			return value_from_<__int128>(entry, to, destination);
		case type_id_type::uint128_:
			return value_from_<unsigned __int128>(entry, to, destination);*/
		case type_id_type::float_:
			return common::controller::active->temporary_storage().add_scalar(value<float>(entry));
		case type_id_type::double_:
			return common::controller::active->temporary_storage().add_scalar(value<double>(entry));
		case type_id_type::ldouble:
			return common::controller::active->temporary_storage().add_scalar(value<long double>(entry));
		default:
			break;
		}
	}

	return object::cast(entry, type, options);
}

void oosl::driver::numeric::echo(entry_type &entry, output_writer_type &writer){
	std::string target;
	value(entry, type_id_type::string_, reinterpret_cast<char *>(&target));
	writer.write(target.c_str());
}

void oosl::driver::numeric::value(entry_type &entry, type_id_type to, char *destination){
	if (OOSL_IS(entry.attributes, attribute_type::nan_)){//NaN
		switch (to){
		case type_id_type::string_:
			*reinterpret_cast<std::string *>(destination) = "NaN";
			break;
		case type_id_type::wstring_:
			*reinterpret_cast<std::wstring *>(destination) = L"NaN";
			break;
		default:
			break;
		}

		throw error_type::not_implemented;
	}

	switch (entry.type->id()){
	case type_id_type::int8_:
		return value_from_<__int8>(entry, to, destination);
	case type_id_type::uint8_:
		return value_from_<unsigned __int8>(entry, to, destination);
	case type_id_type::int16_:
		return value_from_<__int16>(entry, to, destination);
	case type_id_type::uint16_:
		return value_from_<unsigned __int16>(entry, to, destination);
	case type_id_type::int32_:
		return value_from_<__int32>(entry, to, destination);
	case type_id_type::uint32_:
		return value_from_<unsigned __int32>(entry, to, destination);
	case type_id_type::int64_:
		return value_from_<__int64>(entry, to, destination);
	case type_id_type::uint64_:
		return value_from_<unsigned __int64>(entry, to, destination);
	/*case type_id_type::int128_:
		return value_from_<__int128>(entry, to, destination);
	case type_id_type::uint128_:
		return value_from_<unsigned __int128>(entry, to, destination);*/
	case type_id_type::float_:
		return value_from_<float>(entry, to, destination);
	case type_id_type::double_:
		return value_from_<double>(entry, to, destination);
	case type_id_type::ldouble:
		return value_from_<long double>(entry, to, destination);
	default:
		break;
	}

	throw error_type::not_implemented;
}

oosl::driver::object::entry_type *oosl::driver::numeric::evaluate_(entry_type &entry, unary_operator_info_type &operator_info){
	switch (entry.type->id()){
	case type_id_type::int8_:
		return evaluate_integral_<__int8>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::uint8_:
		return evaluate_integral_<unsigned __int8>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::int16_:
		return evaluate_integral_<__int16>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::uint16_:
		return evaluate_integral_<unsigned __int16>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::int32_:
		return evaluate_integral_<__int32>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::uint32_:
		return evaluate_integral_<unsigned __int32>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::int64_:
		return evaluate_integral_<__int64>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::uint64_:
		return evaluate_integral_<unsigned __int64>(entry, operator_info.id, operator_info.is_left);
	/*case type_id_type::int128_:
		return (is_integral ? evaluate_integral_<__int128>(entry, operator_info.id, operator_info.is_left) : evaluate_<__int128>(entry, operator_info.id, operator_info.is_left));
	case type_id_type::uint128_:
		return (is_integral ? evaluate_integral_<unsigned __int128>(entry, operator_info.id, operator_info.is_left) : evaluate_<unsigned __int128>(entry, operator_info.id, operator_info.is_left));*/
	case type_id_type::float_:
		return evaluate_signed_<float>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::double_:
		return evaluate_signed_<double>(entry, operator_info.id, operator_info.is_left);
	case type_id_type::ldouble:
		return evaluate_signed_<long double>(entry, operator_info.id, operator_info.is_left);
	default:
		break;
	}

	return object::evaluate_(entry, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::numeric::evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand){
	auto operand_type = operand.type->driver()->type(operand);
	if (!operand_type->is_numeric()){//Non-numeric operand
		if (operator_info.id != operator_id_type::plus)//Not supported
			return object::evaluate_(entry, operator_info, operand);

		if (operand_type->is_string())
			return concatenate_string_<std::string>(entry, operand, type_id_type::string_);

		if (operand_type->is_wstring())
			return concatenate_string_<std::wstring>(entry, operand, type_id_type::wstring_);
	}

	type_id_type id;
	if (operator_info.id < operator_id_type::compound_plus || operator_info.id > operator_id_type::compound_right_shift)//Use bigger type
		id = ((entry.type->id() < operand_type->id()) ? operand_type->id() : entry.type->id());
	else//Force 'this' type
		id = entry.type->id();

	switch (id){
	case type_id_type::int8_:
		return evaluate_integral_<__int8>(entry, operator_info.id, operand);
	case type_id_type::uint8_:
		return evaluate_integral_<unsigned __int8>(entry, operator_info.id, operand);
	case type_id_type::int16_:
		return evaluate_integral_<__int16>(entry, operator_info.id, operand);
	case type_id_type::uint16_:
		return evaluate_integral_<unsigned __int16>(entry, operator_info.id, operand);
	case type_id_type::int32_:
		return evaluate_integral_<__int32>(entry, operator_info.id, operand);
	case type_id_type::uint32_:
		return evaluate_integral_<unsigned __int32>(entry, operator_info.id, operand);
	case type_id_type::int64_:
		return evaluate_integral_<__int64>(entry, operator_info.id, operand);
	case type_id_type::uint64_:
		return evaluate_integral_<unsigned __int64>(entry, operator_info.id, operand);
	/*case type_id_type::int128_:
		return (is_integral ? evaluate_integral_<__int128>(entry, operator_info.id, operand) : evaluate_<__int128>(entry, operator_info.id, operand));
	case type_id_type::uint128_:
		return (is_integral ? evaluate_integral_<unsigned __int128>(entry, operator_info.id, operand) : evaluate_<unsigned __int128>(entry, operator_info.id, operand));*/
	case type_id_type::float_:
		return evaluate_<float>(entry, operator_info.id, operand);
	case type_id_type::double_:
		return evaluate_<double>(entry, operator_info.id, operand);
	case type_id_type::ldouble:
		return evaluate_<long double>(entry, operator_info.id, operand);
	default:
		break;
	}

	return object::evaluate_(entry, operator_info, operand);
}

bool oosl::driver::numeric::is_(evaluation_option_type left, evaluation_option_type right){
	return OOSL_IS(left, right);
}
