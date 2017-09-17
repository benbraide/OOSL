#include "temporary_storage.h"

oosl::storage::temporary::~temporary(){
	if (!entry_list_.empty()){
		for (auto &entry : entry_list_){
			if (!OOSL_IS(entry.attributes, entry_type::attribute_type::no_free))//Free memory
				common::controller::active->memory().deallocate(entry.address);
		}

		entry_list_.clear();
	}
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(bool_type value){
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

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(const std::string &value){
	return add_string_scalar_(value, common::controller::active->find_type(type::id::string_));
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::add_scalar(const std::wstring &value){
	return add_string_scalar_(value, common::controller::active->find_type(type::id::wstring_));
}

oosl::storage::temporary::entry_type *oosl::storage::temporary::wrap(uint64_type address, type_ptr_type type, entry_attribute_type attributes){
	return &*entry_list_.emplace(entry_list_.end(), entry_type{
		nullptr,
		address,
		attributes,
		type
	});
}
