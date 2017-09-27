#include "controller_impl.h"

oosl::common::controller_impl::controller_impl()
	: states_(state_type::initializing), global_storage_(""), output_stream_writer_(std::cout, std::wcout), error_output_stream_writer_(std::cerr, std::wcerr){
	if (active == nullptr)
		active = this;
	else//Multiple controller instances
		throw error_codes::nil;

	type_list_[type_id_type::any_] = std::make_shared<oosl::type::any>();
	type_list_[type_id_type::auto_] = std::make_shared<oosl::type::primitive>(type_id_type::auto_);
	type_list_[type_id_type::nan_] = std::make_shared<oosl::type::primitive>(type_id_type::nan_);

	for (auto id = type_id_type::undefined; id <= type_id_type::wchar_; id = static_cast<type_id_type>(static_cast<int>(id) + 1))
		type_list_[id] = std::make_shared<oosl::type::primitive>(id);//Add range

	for (auto id = type_id_type::string_; id <= type_id_type::wstring_; id = static_cast<type_id_type>(static_cast<int>(id) + 1))
		type_list_[id] = std::make_shared<oosl::type::primitive>(id);//Add range

	for (auto id = type_id_type::nullptr_; id <= type_id_type::runtime_; id = static_cast<type_id_type>(static_cast<int>(id) + 1))
		type_list_[id] = std::make_shared<oosl::type::primitive>(id);//Add range

	for (auto id = type_id_type::int8_; id <= type_id_type::ldouble; id = static_cast<type_id_type>(static_cast<int>(id) + 1))
		type_list_[id] = std::make_shared<oosl::type::numeric>(id);//Add range

	for (auto id = type_id_type::array_; id <= type_id_type::function_; id = static_cast<type_id_type>(static_cast<int>(id) + 1))
		type_list_[id] = std::make_shared<oosl::type::dynamic>(id);//Add range

	driver_list_[driver_type::boolean] = std::make_shared<oosl::driver::boolean>();
	driver_list_[driver_type::byte] = std::make_shared<oosl::driver::byte>();
	driver_list_[driver_type::char_] = std::make_shared<oosl::driver::char_driver>();
	driver_list_[driver_type::numeric] = std::make_shared<oosl::driver::numeric>();
	driver_list_[driver_type::string] = std::make_shared<oosl::driver::string_driver>();
	driver_list_[driver_type::pointer] = std::make_shared<oosl::driver::pointer>();
	driver_list_[driver_type::dynamic] = std::make_shared<oosl::driver::dynamic_driver>();

	output_writer_list_[output_writer_key_type::nil] = &output_stream_writer_;
	output_writer_list_[output_writer_key_type::error] = &error_output_stream_writer_;

	static_value_list_[static_value_type::false_] = internal_temporary_storage_.add_scalar(oosl::type::bool_type::false_);
	static_value_list_[static_value_type::true_] = internal_temporary_storage_.add_scalar(oosl::type::bool_type::true_);
	static_value_list_[static_value_type::indeterminate] = internal_temporary_storage_.add_scalar(oosl::type::bool_type::indeterminate);

	static_value_list_[static_value_type::nullptr_] = internal_temporary_storage_.add_scalar(nullptr);
	static_value_list_[static_value_type::nan_] = internal_temporary_storage_.add_scalar(oosl::type::other_type::nan_);
	static_value_list_[static_value_type::undefined] = internal_temporary_storage_.add_scalar(oosl::type::other_type::undefined);

	OOSL_REMOVE(states_, state_type::initializing);
}

oosl::common::controller_impl::~controller_impl(){
	
}

bool oosl::common::controller_impl::exiting(){
	return OOSL_IS(states_, state_type::exiting);
}

bool oosl::common::controller_impl::initializing(){
	return OOSL_IS(states_, state_type::initializing);
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

oosl::common::controller::output_writer_type &oosl::common::controller_impl::output_writer(output_writer_key_type type){
	auto entry = output_writer_list_.find(type);
	if (entry == output_writer_list_.end())//Not found
		throw error_codes::writer_not_found;

	return *entry->second;
}

void oosl::common::controller_impl::output_writer(output_writer_key_type type, output_writer_type &value){
	output_writer_list_[type] = &value;
}

void oosl::common::controller_impl::restore_output_writer(output_writer_key_type type){
	switch (type){
	case output_writer_key_type::nil:
		output_writer_list_[type] = &output_stream_writer_;
		break;
	case output_writer_key_type::error:
		output_writer_list_[type] = &error_output_stream_writer_;
		break;
	default:
		break;
	}
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
	if (entry == driver_list_.end())
		throw error_codes::driver_not_found;

	return entry->second.get();
}

void oosl::common::controller_impl::exit(){
	OOSL_SET(states_, state_type::exiting);
	memory_manager_.tear();
}

thread_local oosl::common::controller::interpreter_info_type oosl::common::controller_impl::interpreter_info_{};

thread_local oosl::common::controller::runtime_info_type oosl::common::controller_impl::runtime_info_{ oosl::storage::object::find_type::recursive };
