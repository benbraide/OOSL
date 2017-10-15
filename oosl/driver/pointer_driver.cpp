#include "pointer_driver.h"

oosl::driver::pointer::~pointer() = default;

oosl::driver::object::entry_type *oosl::driver::pointer::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	if (OOSL_IS(options, cast_option_type::ref) || type.is_ref())
		return object::cast(entry, type, options);//Reference cast

	if (OOSL_IS(options, cast_option_type::reinterpret)){//Reinterpret cast
		if (type.is_integral()){//Cast to integral value
			switch (type.id()){
			case type_id_type::int8_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<__int8>(value<uint64_type>(entry)));
			case type_id_type::uint8_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int8>(value<uint64_type>(entry)));
			case type_id_type::int16_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<__int16>(value<uint64_type>(entry)));
			case type_id_type::uint16_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int16>(value<uint64_type>(entry)));
			case type_id_type::int32_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<__int32>(value<uint64_type>(entry)));
			case type_id_type::uint32_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int32>(value<uint64_type>(entry)));
			case type_id_type::int64_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<__int64>(value<uint64_type>(entry)));
			case type_id_type::uint64_:
				return common::controller::active->temporary_storage().add_scalar(static_cast<unsigned __int64>(value<uint64_type>(entry)));
			default:
				break;
			}
		}
		else if (type.is_strong_pointer() && !entry.type->is_same(type))//Cast to 
			return common::controller::active->temporary_storage().add_scalar(value<uint64_type>(entry), type.reflect());
	}

	return object::cast(entry, type, options);
}

oosl::driver::object::entry_type *oosl::driver::pointer::evaluate(entry_type &entry, operator_info_type &operator_info, node_ptr_type &operand){
	if (operator_info.id == operator_id_type::member_pointer_access){
		auto target = target_entry(entry);
		operator_info_type member_access{ operator_id_type::member_access };
		return target->type->driver()->evaluate(*target, member_access, operand);
	}

	return object::evaluate(entry, operator_info, operand);
}

void oosl::driver::pointer::echo(entry_type &entry, output_writer_type &writer){
	if (OOSL_IS(entry.attributes, attribute_type::uninitialized)){//Uninitialized
		writer.write("<undefined>");
		return;
	}

	auto target_value = value<uint64_type>(entry);
	if (target_value == 0u){//Null pointer
		writer.write("nullptr");
		return;
	}

	auto target = target_entry(entry);
	auto string_value = common::controller::convert_base(target_value, 16, (sizeof(uint64_type) << 1));

	writer.write("<");
	writer.write(string_value.c_str());
	writer.write(": ");

	target->type->driver()->echo(*target, writer);
	writer.write(">");
}

void oosl::driver::pointer::value(entry_type &entry, type_id_type to, char *destination){
	if (to == type_id_type::uint64_){//Copy pointer value
		auto value = oosl::common::controller::active->memory().read<uint64_type>(entry.address);
		memcpy(destination, &value, sizeof(uint64_type));
	}
}

oosl::driver::object::entry_type *oosl::driver::pointer::target_entry(entry_type &entry, long long offset){
	auto address = value<uint64_type>(entry);
	if (offset < 0ll)//Decrement
		address -= -pre_multiply(entry, offset);
	else//Increment
		address += pre_multiply(entry, offset);

	return oosl::common::controller::active->temporary_storage().wrap(address, entry.type->underlying_type()->reflect(), attribute_type::lval);
}

long long oosl::driver::pointer::pre_multiply(entry_type &entry, long long value){
	return (value * static_cast<long long>(entry.type->underlying_type()->size()));
}

oosl::driver::object::entry_type *oosl::driver::pointer::evaluate_(entry_type &entry, operator_info_type &operator_info){
	switch (operator_info.id){
	case operator_id_type::times://Dereference
		return target_entry(entry);
	case operator_id_type::decrement:
		return post_evaluate_(entry, (value<uint64_type>(entry) - pre_multiply(entry, 1)), post_evaluation_type::assign);
	case operator_id_type::increment:
		return post_evaluate_(entry, (value<uint64_type>(entry) + pre_multiply(entry, 1)), post_evaluation_type::assign);
	default:
		break;
	}

	return object::evaluate_(entry, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::pointer::evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand){
	auto operand_type = operand.type->driver()->type(operand);
	if (operand_type->is_integral()){//Integral evaluation
		switch (operator_info.id){
		case operator_id_type::index:
			return target_entry(entry, operand.type->driver()->value<long long>(operand));
		case operator_id_type::plus:
			return offset_entry_(entry, operand.type->driver()->value<long long>(operand));
		case operator_id_type::compound_plus:
			return offset_entry_(entry, operand.type->driver()->value<long long>(operand), true);
		case operator_id_type::minus:
			return offset_entry_(entry, -operand.type->driver()->value<long long>(operand));
		case operator_id_type::compound_minus:
			return offset_entry_(entry, -operand.type->driver()->value<long long>(operand), true);
		default:
			break;
		}

		return object::evaluate_(entry, operator_info, operand);
	}

	if (entry.type->is_same(*operand_type))
		return evaluate_boolean_<uint64_type>(entry, operator_info.id, operand);

	return object::evaluate_(entry, operator_info, operand);
}

oosl::driver::object::entry_type *oosl::driver::pointer::offset_entry_(entry_type &entry, long long offset, bool assign){
	auto address = value<uint64_type>(entry);
	if (offset < 0ll)//Decrement
		address -= pre_multiply(entry, -offset);
	else//Increment
		address += pre_multiply(entry, offset);

	if (!assign)//Temporary value
		return oosl::common::controller::active->temporary_storage().add_scalar(address, entry.type);

	auto block = oosl::common::controller::active->memory().find_block(entry.address);
	if (block == nullptr)//Error
		throw error_type::invalid_address;

	memcpy(block->ptr, &address, block->size);
	return &entry;
}
