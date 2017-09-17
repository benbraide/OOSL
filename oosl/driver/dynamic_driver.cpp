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

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::evaluate(entry_type &entry, unary_operator_info_type &operator_info){
	auto active_controller = common::controller::active;
	auto &temp_storage = active_controller->temporary_storage();

	if (operator_info.is_left){
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
	}

	auto linked_target = linked(entry);
	return linked_target->type->driver()->evaluate(*linked_target, operator_info);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::evaluate(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand){
	if (operator_info.id == operator_id_type::assignment)
		return assign(entry, operand);//Assign

	auto linked_target = linked(entry);
	return linked_target->type->driver()->evaluate(*linked_target, operator_info, operand);
}

void oosl::driver::dynamic_driver::initialize(entry_type &entry){}

void oosl::driver::dynamic_driver::echo(entry_type &entry, output_writer_type &writer){
	auto linked_target = linked(entry);
	linked_target->type->driver()->echo(*linked_target);
}

void oosl::driver::dynamic_driver::value(entry_type &entry, type_id_type to, char *destination){
	auto linked_target = linked(entry);
	return linked_target->type->driver()->value(*linked_target, to, destination);
}

oosl::driver::object::entry_type *oosl::driver::dynamic_driver::assign_(entry_type &entry, entry_type &value){
	auto dependency = common::controller::active->memory().find_dependency(entry.address);
	if (dependency == nullptr){//Set dependency
		common::controller::active->memory().add_dependency(entry.address, (dependency = std::make_shared<dependency_type>(*duplicate(value))));
		OOSL_REMOVE(entry.attributes, attribute_type::uninitialized);
	}
	else//Update value
		dynamic_cast<dependency_type *>(dependency.get())->value() = *duplicate(value);

	return &entry;
}
