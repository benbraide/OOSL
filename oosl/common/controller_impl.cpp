#include "controller_impl.h"

oosl::common::controller_impl::controller_impl()
	: global_storage_(""){
	if (active == nullptr)
		active = this;
	else//Multiple controller instances
		throw error_codes::nil;
}

oosl::common::controller_impl::~controller_impl(){

}

bool oosl::common::controller_impl::exiting(){
	throw error_codes::not_implemented;
}

oosl::common::controller::interpreter_info_type &oosl::common::controller_impl::interpreter_info(){
	throw error_codes::not_implemented;
}

oosl::common::controller::runtime_info_type &oosl::common::controller_impl::runtime_info(){
	return runtime_info_;
}

void oosl::common::controller_impl::on_exception(callback_type callback){
	throw error_codes::not_implemented;
}

void oosl::common::controller_impl::on_exception_pop(){
	throw error_codes::not_implemented;
}

oosl::common::controller::output_writer_type &oosl::common::controller_impl::output_writer(){
	throw error_codes::not_implemented;
}

oosl::common::controller::output_writer_type &oosl::common::controller_impl::error_output_writer(){
	throw error_codes::not_implemented;
}

oosl::common::controller::memory_manager_type &oosl::common::controller_impl::memory(){
	return memory_manager_;
}

oosl::common::controller::storage_object_type &oosl::common::controller_impl::global_storage(){
	return global_storage_;
}

oosl::common::controller::temporary_storage_type &oosl::common::controller_impl::temporary_storage(){
	return *runtime_info_.temporary_storage;
}

oosl::common::controller::type_object_ptr_type oosl::common::controller_impl::find_type(type_id_type id){
	auto entry = type_list_.find(id);
	return ((entry == type_list_.end()) ? nullptr : entry->second);
}

oosl::common::controller::storage_entry_type *oosl::common::controller_impl::find_static_value(static_value_type type){
	auto entry = static_value_list_.find(type);
	return ((entry == static_value_list_.end()) ? nullptr : entry->second);
}

oosl::common::controller::driver_object_type *oosl::common::controller_impl::find_driver(driver_type type){
	auto entry = driver_list_.find(type);
	return ((entry == driver_list_.end()) ? nullptr : &entry->second);
}

thread_local oosl::common::controller::runtime_info_type oosl::common::controller_impl::runtime_info_{};
