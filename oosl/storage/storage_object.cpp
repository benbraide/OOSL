#include "storage_object.h"

#include "../type/type_object.h"
#include "../driver/driver_object.h"

oosl::storage::object::object(object *parent)
	: parent_(parent), inside_destructor_(false){}

oosl::storage::object::~object(){
	if (!common::controller::active->exiting()){
		lock_once_type guard(common::controller::active->memory().lock());

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
	auto iter = find_(key);
	if (iter != value_list_.end())//Found
		return iter->second.get();
	return ((type == find_type::recursive && parent_ != nullptr) ? parent_->find(key, type) : nullptr);
}

void oosl::storage::object::remove(const std::string &key){
	lock_once_type guard(common::controller::active->memory().lock());
	remove_(find_(key));
}

void oosl::storage::object::remove(value_type *value){
	lock_once_type guard(common::controller::active->memory().lock());
	remove_(find_(value));
}

void oosl::storage::object::use(const std::string &key, value_ptr_type value){
	lock_once_type guard(common::controller::active->memory().lock());
	use_(key, value);
}

void oosl::storage::object::use(object &storage){
	lock_once_type guard(common::controller::active->memory().lock());
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

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_(const std::string &key){
	lock_once_type guard(common::controller::active->memory().lock(), memory::manager::shared_locker);
	return value_list_.find(key);
}

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_(value_type *value){
	lock_once_type guard(common::controller::active->memory().lock(), memory::manager::shared_locker);
	for (auto iter = value_list_.begin(); iter != value_list_.end(); ++iter){
		if (&*iter->second == value)
			return iter;
	}

	return value_list_.end();
}
