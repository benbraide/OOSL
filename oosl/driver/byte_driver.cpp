#include "byte_driver.h"

oosl::driver::byte::~byte() = default;

oosl::driver::object::entry_type *oosl::driver::byte::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	if (!OOSL_IS_ANY(options, cast_option_type::reinterpret | cast_option_type::ref) && !type.is_ref()){
		switch (type.id()){
		case type_id_type::int8_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<__int8>(value<uint8_type>(entry)));
		case type_id_type::uint8_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int8>(value<uint8_type>(entry)));
		case type_id_type::int16_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<__int16>(value<uint8_type>(entry)));
		case type_id_type::uint16_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int16>(value<uint8_type>(entry)));
		case type_id_type::int32_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<__int32>(value<uint8_type>(entry)));
		case type_id_type::uint32_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int32>(value<uint8_type>(entry)));
		case type_id_type::int64_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<__int64>(value<uint8_type>(entry)));
		case type_id_type::uint64_:
			return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int64>(value<uint8_type>(entry)));
		default:
			break;
		}
	}

	return object::cast(entry, type, options);
}

void oosl::driver::byte::echo(entry_type &entry, output_writer_type &writer){
	auto string_value = common::controller::convert_base(value<uint8_type>(entry), 16, (sizeof(uint8_type) << 1));
	writer.write(string_value.c_str());
}

void oosl::driver::byte::value(entry_type &entry, type_id_type to, char *destination){
	if (to == type_id_type::byte_ || to == type_id_type::uint8_){
		auto value = oosl::common::controller::active->memory().read<uint8_type>(entry.address);
		memcpy(destination, &value, sizeof(uint8_type));
	}
}

oosl::driver::object::entry_type *oosl::driver::byte::evaluate_(entry_type &entry, operator_info_type &operator_info){
	static auto base_options = (post_evaluation_type::byte | post_evaluation_type::assign);
	switch (operator_info.id){
	case operator_id_type::bitwise_inverse:
		return post_evaluate_(entry, ~value<uint8_type>(entry), post_evaluation_type::byte);
	case operator_id_type::decrement:
		return post_evaluate_(entry, (value<uint8_type>(entry) - static_cast<uint8_type>(1)), base_options);
	case operator_id_type::increment:
		return post_evaluate_(entry, (value<uint8_type>(entry) + static_cast<uint8_type>(1)), base_options);
	default:
		break;
	}

	return object::evaluate_(entry, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::byte::evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand){
	return evaluate_(entry, operator_info.id, operand.type->driver()->value<uint8_type>(operand));
}

oosl::driver::object::entry_type *oosl::driver::byte::evaluate_(entry_type &entry, operator_id_type operator_id, uint8_type operand){
	switch (operator_id){
	case operator_id_type::left_shift:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) << operand), post_evaluation_type::byte);
	case operator_id_type::compound_left_shift:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) << operand), (post_evaluation_type::byte | post_evaluation_type::assign));
	case operator_id_type::right_shift:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) >> operand), post_evaluation_type::byte);
	case operator_id_type::compound_right_shift:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) >> operand), (post_evaluation_type::byte | post_evaluation_type::assign));
	case operator_id_type::bitwise_and:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) & operand), post_evaluation_type::byte);
	case operator_id_type::compound_bitwise_and:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) & operand), (post_evaluation_type::byte | post_evaluation_type::assign));
	case operator_id_type::bitwise_or:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) | operand), post_evaluation_type::byte);
	case operator_id_type::compound_bitwise_or:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) | operand), (post_evaluation_type::byte | post_evaluation_type::assign));
	case operator_id_type::bitwise_xor:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) ^ operand), post_evaluation_type::byte);
	case operator_id_type::compound_bitwise_xor:
		return post_evaluate_(entry, static_cast<uint8_type>(value<uint8_type>(entry) ^ operand), (post_evaluation_type::byte | post_evaluation_type::assign));
	default:
		break;
	}

	return evaluate_boolean_<uint8_type>(entry, operator_id, operand);
}
