#include "driver_object.h"

oosl::driver::object::~object() = default;

oosl::driver::object::entry_type *oosl::driver::object::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	auto is_rval_ref = (OOSL_IS(options, cast_option_type::val) || type.is_rval_ref());
	auto is_ref = (OOSL_IS(options, cast_option_type::ref) || type.is_ref()), is_lval = this->is_lval(entry);

	if (is_ref){
		if (!is_lval && !is_rval_ref)
			throw error_type::rval_to_lval;

		if (type.is_any() || type.non_modified()->is_same(*this->type(entry)->non_modified()))
			return &entry;

		if (!is_rval_ref)//Types must be the same
			throw error_type::incompatible_types;
	}

	if (is_uninitialized(entry)){//Only reference cast is permitted
		if (!is_ref || !is_lval)
			throw error_type::uninitialized_object;

		if (type.is_any() || type.non_modified()->is_same(*this->type(entry)->non_modified()))
			return &entry;

		throw error_type::uninitialized_object;
	}

	if (OOSL_IS(options, cast_option_type::reinterpret))
		throw error_type::not_implemented;//Cannot perform reinterpret cast

	if (!type.is_any() && !type.non_modified()->is_same(*this->type(entry)->non_modified()))
		throw error_type::incompatible_types;

	if (OOSL_IS(options, cast_option_type::make_copy)){

	}

	return &entry;
}

oosl::driver::object::type_object_type *oosl::driver::object::type(entry_type &entry){
	return entry.type.get();
}

oosl::driver::object::size_type oosl::driver::object::size(entry_type &entry){
	return entry.type->size();
}

oosl::driver::object::uint64_type oosl::driver::object::address(entry_type &entry){
	return entry.address;
}

oosl::driver::object::entry_type *oosl::driver::object::linked(entry_type &entry){
	throw error_type::not_implemented;
}

oosl::driver::object::entry_type *oosl::driver::object::duplicate(entry_type &entry){
	auto entry_block = common::controller::active->memory().find_block(entry.address);
	if (entry_block == nullptr)//Error
		throw error_type::invalid_address;

	if (!OOSL_IS(entry.attributes, attribute_type::lval)){//Use temporary value
		++entry_block->ref_count;//Prevent owner storage from deleting memory
		return &entry;
	}

	auto copied_value = common::controller::active->temporary_storage().add(entry_block->actual_size);
	auto copied_value_block = common::controller::active->memory().find_block(copied_value->address);

	copied_value->type = entry.type;
	memcpy(copied_value_block->ptr, entry_block->ptr, entry_block->actual_size);

	OOSL_SET(copied_value_block->attributes, entry_block->attributes);
	++copied_value_block->ref_count;//Prevent temporary storage from deleting memory

	return copied_value;
}

oosl::driver::object::attribute_type oosl::driver::object::attributes(entry_type &entry){
	auto attributes = entry.attributes;
	if (entry.type->is_ref())
		OOSL_SET(attributes, attribute_type::lval | attribute_type::ref_);

	if (entry.type->is_thread_local())
		OOSL_SET(attributes, attribute_type::tls);

	if (entry.type->is_nan())
		OOSL_SET(attributes, attribute_type::nan_);

	if (entry.type->is_void())
		OOSL_SET(attributes, attribute_type::void_);

	return attributes;
}

oosl::driver::object::entry_type *oosl::driver::object::evaluate(entry_type &entry, operator_info_type &operator_info){
	if (entry.type->is_void())
		throw error_type::void_object;

	auto active_controller = common::controller::active;
	auto &temp_storage = active_controller->temporary_storage();

	switch (operator_info.id){
	case operator_id_type::bitwise_and://Return pointer to address
		if (!OOSL_IS(entry.attributes, attribute_type::lval))
			throw error_type::rval_ref;
		return temp_storage.add_scalar(entry.address, std::make_shared<oosl::type::pointer>(entry.type->non_modified()->reflect()));
	case operator_id_type::sizeof_:
		return temp_storage.add_scalar(size(entry));
	case operator_id_type::typeof:
		return temp_storage.add_scalar(*type(entry));
	case operator_id_type::call://(this)
		return &entry;
	default:
		break;
	}

	if (OOSL_IS(entry.attributes, attribute_type::uninitialized))
		throw error_type::uninitialized_object;

	return evaluate_(entry, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::object::evaluate(entry_type &entry, operator_info_type &operator_info, entry_type &operand){
	if (operator_info.id == operator_id_type::assignment)
		return assign(entry, operand);//Assign

	if (entry.type->is_void() || operand.type->is_void())
		throw error_type::void_object;

	if (OOSL_IS(entry.attributes, attribute_type::uninitialized) || OOSL_IS(operand.attributes, attribute_type::uninitialized))
		throw error_type::uninitialized_object;

	if (operator_info.id == operator_id_type::comma)
		return operand.type->driver()->cast(operand, *type(entry), cast_option_type::ref);

	return evaluate_(entry, operator_info, operand);
}

oosl::driver::object::entry_type *oosl::driver::object::evaluate(entry_type &entry, operator_info_type &operator_info, node_ptr_type &operand){
	throw error_type::unhandled_operator;
}

oosl::driver::object::entry_type *oosl::driver::object::assign(entry_type &entry, entry_type &value){
	if (entry.type->is_void() || value.type->is_void())
		throw error_type::void_object;

	if (!OOSL_IS(entry.attributes, attribute_type::lval))
		throw error_type::rval_assignment;

	return assign_(entry, value);
}

void oosl::driver::object::initialize(entry_type &entry){
	if (!OOSL_IS(entry.attributes, attribute_type::uninitialized)){
		oosl::common::controller::active->memory().fill(entry.address, '\0', size(entry));
		OOSL_SET(entry.attributes, attribute_type::uninitialized);
	}
}

bool oosl::driver::object::is_lval(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::lval) || entry.type->is_ref());
}

bool oosl::driver::object::is_ref(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::ref_) || entry.type->is_ref());
}

bool oosl::driver::object::is_uninitialized(entry_type &entry){
	return OOSL_IS(entry.attributes, attribute_type::uninitialized);
}

bool oosl::driver::object::is_tls(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::tls) || entry.type->is_thread_local());
}

bool oosl::driver::object::is_nan(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::nan_) || entry.type->is_nan());
}

bool oosl::driver::object::is_void(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::void_) || entry.type->is_void());
}

void oosl::driver::object::echo(entry_type &entry){
	echo(entry, common::controller::active->output_writer(output_writer_key_type::nil));
}

void oosl::driver::object::echo(entry_type &entry, output_writer_type &writer){
	writer.write(("<Object: " + entry.type->print() + ">").c_str());
}

void oosl::driver::object::value(entry_type &entry, type_id_type to, char *destination){
	throw error_type::not_implemented;
}

oosl::driver::object::entry_type *oosl::driver::object::evaluate_(entry_type &entry, operator_info_type &operator_info){
	throw error_type::unhandled_operator;
}

oosl::driver::object::entry_type *oosl::driver::object::evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand){
	throw error_type::unhandled_operator;
}

oosl::driver::object::entry_type *oosl::driver::object::assign_(entry_type &entry, entry_type &value){
	auto converted_value = value.type->driver()->cast(value, *entry.type);
	oosl::common::controller::active->memory().copy(entry.address, converted_value->address, entry.type->size());
	OOSL_REMOVE(entry.attributes, attribute_type::uninitialized);
	return &entry;
}
