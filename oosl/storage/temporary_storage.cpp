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
