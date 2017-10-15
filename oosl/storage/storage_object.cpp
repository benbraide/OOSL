#include "storage_object.h"

#include "../type/type_object.h"
#include "../driver/driver_object.h"
#include "../common/structures.h"

oosl::storage::object::object(object *parent)
	: parent_(parent), inside_destructor_(false){}

oosl::storage::object::~object(){
	if (!common::controller::active->exiting()){
		guard_type guard(lock_);

		inside_destructor_ = true;
		if (!order_list_.empty()){
			for (auto entry : order_list_)//Remove items by order
				remove_(find_(*entry.first));
			order_list_.clear();
		}

		if (!value_list_.empty())
			value_list_.clear();
	}
}

oosl::storage::object *oosl::storage::object::parent(){
	return parent_;
}

oosl::storage::object *oosl::storage::object::match(const std::string &name){
	return ((parent_ == nullptr) ? nullptr : parent_->match(name));
}

oosl::storage::object::value_type *oosl::storage::object::find(const std::string &key, find_type type){
	return ((common::controller::active->runtime_info().find_target == nullptr) ? do_find_(key, type) : nullptr);
}

void oosl::storage::object::remove(const std::string &key){
	guard_type guard(lock_);
	remove_(find_(key));
}

void oosl::storage::object::remove(value_type *value){
	guard_type guard(lock_);
	remove_(find_(value));
}

void oosl::storage::object::use(const std::string &key, value_ptr_type value){
	guard_type guard(lock_);
	use_(key, value);
}

void oosl::storage::object::use(object &storage){
	guard_type guard(lock_);
	for (auto &entry : storage.value_list_)
		use_(entry.first, entry.second);
}

void oosl::storage::object::use_(const std::string &key, value_ptr_type value){
	value_list_[key] = value;
}

void oosl::storage::object::remove_(value_list_iterator_type iter){
	if (iter == value_list_.end())
		return;//Entry not found

	if (!inside_destructor_){
		for (auto entry = order_list_.begin(); entry != order_list_.end(); ++entry){
			if (entry->second == &*iter->second){//Erase from order list
				order_list_.erase(entry);
				break;
			}
		}
	}

	auto object_value = iter->second->object();
	if (object_value != nullptr && !OOSL_IS(object_value->attributes, entry_type::attribute_type::no_free))
		common::controller::active->memory().deallocate(object_value->address);//Free memory

	if (!inside_destructor_)
		value_list_.erase(iter);//Erase from list
}

oosl::storage::object::value_type *oosl::storage::object::do_find_(const std::string &key, find_type type){
	auto iter = find_locked_(key);
	if (iter != value_list_.end())//Found
		return iter->second.get();
	return ((type == find_type::recursive && parent_ != nullptr) ? parent_->find(key, type) : nullptr);
}

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_locked_(const std::string &key){
	shared_guard_type guard(lock_);
	return find_(key);
}

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_(const std::string &key){
	return value_list_.find(key);
}

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_(value_type *value){
	for (auto iter = value_list_.begin(); iter != value_list_.end(); ++iter){
		if (iter->second.get() == value)
			return iter;
	}

	return value_list_.end();
}
