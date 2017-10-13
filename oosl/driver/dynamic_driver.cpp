#include "dynamic_driver.h"

oosl::driver::dynamic_driver::~dynamic_driver() = default;

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	auto linked_target = linked(entry);
	return linked_target->type->driver()->cast(*linked_target, type, options);
}

oosl::driver::object::type_object_type *oosl::driver::dynamic_driver::type(entry_type &entry){
	return (OOSL_IS(entry.attributes, attribute_type::uninitialized) ? entry.type.get() : linked(entry)->type.get());
}

oosl::driver::object::size_type oosl::driver::dynamic_driver::size(entry_type &entry){
	auto linked_target = linked(entry);
	return linked_target->type->driver()->size(*linked_target);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::linked(entry_type &entry){
	if (OOSL_IS(entry.attributes, attribute_type::uninitialized))
		throw error_type::uninitialized_object;

	auto dependency = common::controller::active->memory().find_dependency(entry.address);
	if (dependency == nullptr)//Error
		throw error_type::invalid_address;

	return &dynamic_cast<dependency_type *>(dependency.get())->value();
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::evaluate(entry_type &entry, operator_info_type &operator_info){
	auto active_controller = common::controller::active;
	auto &temp_storage = active_controller->temporary_storage();

	switch (operator_info.id){
	case operator_id_type::bitwise_and://Return pointer to address
		if (!OOSL_IS(entry.attributes, attribute_type::lval))
			throw error_type::rval_ref;
		return temp_storage.add_scalar(entry.address, std::make_shared<oosl::type::pointer>(entry.type->non_modified()->reflect()));
	case operator_id_type::call://(this)
		return &entry;
	default:
		break;
	}

	auto linked_target = linked(entry);
	return linked_target->type->driver()->evaluate(*linked_target, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::evaluate(entry_type &entry, operator_info_type &operator_info, entry_type &operand){
	if (operator_info.id == operator_id_type::assignment)
		return assign(entry, operand);//Assign

	auto linked_target = linked(entry);
	return linked_target->type->driver()->evaluate(*linked_target, operator_info, operand);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::assign(entry_type &entry, entry_type &value){
	if (!OOSL_IS(entry.attributes, attribute_type::uninitialized)){//Linked assignment
		auto linked_target = linked(entry);
		return linked_target->type->driver()->assign(*linked_target, value);
	}

	return object::assign(entry, value);
}

void oosl::driver::dynamic_driver::initialize(entry_type &entry){}

void oosl::driver::dynamic_driver::echo(entry_type &entry, output_writer_type &writer){
	if (!OOSL_IS(entry.attributes, attribute_type::uninitialized)){
		auto linked_target = linked(entry);
		linked_target->type->driver()->echo(*linked_target);
	}
	else//Uninitialized
		writer.write("<undefined>");
}

void oosl::driver::dynamic_driver::value(entry_type &entry, type_id_type to, char *destination){
	auto linked_target = linked(entry);
	return linked_target->type->driver()->value(*linked_target, to, destination);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::assign_(entry_type &entry, entry_type &value){
	if (entry.type->is_ref()){
		auto value_type = value.type->driver()->type(value);
		if (entry.type->score(*value_type) < (OOSL_MAX_TYPE_SCORE - 2))
			throw error_type::unhandled_operator;
	}
	
	auto dependency = common::controller::active->memory().find_dependency(entry.address);
	if (dependency == nullptr){//Set dependency
		if (entry.type->is_ref())
			common::controller::active->memory().add_dependency(entry.address, (dependency = std::make_shared<dependency_type>(value)));
		else//Non-reference
			common::controller::active->memory().add_dependency(entry.address, (dependency = std::make_shared<dependency_type>(*duplicate(value))));
		OOSL_REMOVE(entry.attributes, attribute_type::uninitialized);
	}
	else if (entry.type->is_ref())//Reference update
		dynamic_cast<dependency_type *>(dependency.get())->value() = value;
	else//Update value
		dynamic_cast<dependency_type *>(dependency.get())->value() = *duplicate(value);

	return &entry;
}
