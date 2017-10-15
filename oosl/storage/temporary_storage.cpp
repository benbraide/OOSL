#include "temporary_storage.h"

oosl::storage::temporary::~temporary(){
	if (!common::controller::active->exiting() && !entry_list_.empty()){
		for (auto &entry : entry_list_){
			if (!OOSL_IS(entry.attributes, entry_type::attribute_type::no_free))//Free memory
				common::controller::active->memory().deallocate(entry.address);
		}

		entry_list_.clear();
	}
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(std::nullptr_t){
	if (common::controller::active->initializing())//Controller is initializing
		return add_scalar_(static_cast<uint64_type>(0), common::controller::active->find_type(type_id_type::nullptr_));
	return common::controller::active->find_static_value(common::controller::static_value_type::nullptr_);
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(bool value){
	return add_scalar(value ? bool_type::true_ : bool_type::false_);
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(bool_type value){
	if (common::controller::active->initializing())//Controller is initializing
		return add_scalar_(value, common::controller::active->find_type(type_id_type::bool_));

	switch (value){
	case bool_type::false_:
		return common::controller::active->find_static_value(common::controller::static_value_type::false_);
	case bool_type::true_:
		return common::controller::active->find_static_value(common::controller::static_value_type::true_);
	default:
		break;
	}

	return common::controller::active->find_static_value(common::controller::static_value_type::indeterminate);
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(other_type value){
	switch (value){
	case other_type::nan_:
		break;
	case other_type::undefined:
		if (common::controller::active->initializing())//Controller is initializing
			return add_scalar_(static_cast<uint64_type>(0), common::controller::active->find_type(type_id_type::undefined));
		return common::controller::active->find_static_value(common::controller::static_value_type::undefined);
	default:
		throw common::error_codes::bad_constant;
		break;
	}

	if (common::controller::active->initializing()){//Controller is initializing
		auto entry = add_scalar_(static_cast<uint64_type>(0), common::controller::active->find_type(type_id_type::nan_));
		OOSL_SET(entry->attributes, entry_type::attribute_type::nan_);
		return entry;
	}

	return common::controller::active->find_static_value(common::controller::static_value_type::nan_);
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(const std::string &value){
	return add_string_scalar_(value, common::controller::active->find_type(type::id::string_));
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(const std::wstring &value){
	return add_string_scalar_(value, common::controller::active->find_type(type::id::wstring_));
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(type_object_type &value){
	auto entry = add_scalar_(reinterpret_cast<uint64_type>(&value), common::controller::active->find_type(type_id_type::type_));
	common::controller::active->memory().add_dependency(entry->address, std::make_shared<dependency_type>(value.reflect()));
	return entry;
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(storage_object_type &value){
	return add_scalar_(reinterpret_cast<uint64_type>(&value), common::controller::active->find_type(type_id_type::storage_));
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::wrap(uint64_type address, type_ptr_type type, entry_attribute_type attributes){
	return &*entry_list_.emplace(entry_list_.end(), entry_type{
		nullptr,
		address,
		attributes,
		type
	});
}
