#include "storage_object.h"

#include "../type/type_object.h"
#include "../node/node_object.h"
#include "../driver/driver_object.h"
#include "../common/structures.h"

oosl::storage::storage_tls_entry::value_type *oosl::storage::storage_tls_entry::find(const std::string &key){
	auto entry = value_list_.find(key);
	return ((entry == value_list_.end()) ? nullptr : entry->second.get());
}

oosl::storage::storage_tls_entry::value_type *oosl::storage::storage_tls_entry::add(const std::string &key, value_ptr_type value){
	return (value_list_[key] = value).get();
}

oosl::storage::object::object(object *parent)
	: parent_(parent), inside_destructor_(false){}

oosl::storage::object::~object(){
	if (!common::controller::active->exiting()){
		guard_type guard(lock_);

		inside_destructor_ = true;
		if (!order_list_.empty()){
			for (auto entry : order_list_)//Remove items by order
				remove_(value_list_.find(*entry.first));
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

oosl::storage::object::value_type *oosl::storage::object::add(const std::string &key, entry_type &entry, node_ptr_type node){
	typedef entry_value<node_ptr_type> entry_value_type;
	if (OOSL_IS(common::controller::active->runtime_info().states, common::runtime_state::tls_init)){//Add thread-local entry
		auto tls_entry = common::controller::active->tls().get<storage_tls_entry>(reinterpret_cast<uint64_type>(this));
		if (tls_entry == nullptr && (tls_entry = common::controller::active->tls().add<storage_tls_entry>(reinterpret_cast<uint64_type>(this))) == nullptr)//Add entry
			throw common::error_codes::out_of_memory;

		return (common::controller::active->runtime_info().tls_value = tls_entry->add(key, std::make_shared<value>(entry)));
	}

	if (value_list_.find(key) != value_list_.end())
		throw common::error_codes::redefinition;

	auto value = add_<entry_value_type>(key, node, entry);
	if (!OOSL_IS(entry.attributes, entry_type::attribute_type::tls))
		return value;//Not thread-local

	auto &runtime_state = common::controller::active->runtime_info().states;
	common::raii_state<common::runtime_state> runtime_state_raii(runtime_state, (runtime_state | common::runtime_state::tls_init));

	return add(key, entry, node);//Add thread-local entry
}

void oosl::storage::object::remove(const std::string &key){
	guard_type guard(lock_);
	remove_(value_list_.find(key));
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
	auto value = find_locked_(key);
	if (value != nullptr)//Found
		return value;
	return ((type == find_type::recursive && parent_ != nullptr) ? parent_->find(key, type) : nullptr);
}

oosl::storage::object::value_type *oosl::storage::object::find_locked_(const std::string &key){
	shared_guard_type guard(lock_);
	return find_(key);
}

oosl::storage::object::value_type *oosl::storage::object::find_(const std::string &key){
	auto value = value_list_.find(key);
	if (value == value_list_.end())//Not found
		return nullptr;

	auto entry = value->second->object();
	if (entry == nullptr || !OOSL_IS(entry->attributes, entry_type::attribute_type::tls))//Not an object | thread-local
		return value->second.get();

	auto tls_entry = common::controller::active->tls().get<storage_tls_entry>(reinterpret_cast<uint64_type>(this));
	auto tls_value = ((tls_entry == nullptr) ? nullptr : tls_entry->find(key));

	if (tls_value != nullptr)
		return tls_value;//Use thread-local value

	auto node_value = dynamic_cast<entry_value<node_ptr_type> *>(value->second.get());
	if (node_value == nullptr)//Error
		throw common::error_codes::out_of_memory;

	auto &runtime_state = common::controller::active->runtime_info().states;
	common::raii_state<common::runtime_state> runtime_state_raii(runtime_state, (runtime_state | common::runtime_state::tls_init));
	common::raii_state<object *> runtime_storage_raii(common::controller::active->runtime_info().storage, this);

	node_value->node()->evaluate();//Initialize thread-local value
	return common::controller::active->runtime_info().tls_value;
}

oosl::storage::object::value_list_iterator_type oosl::storage::object::find_(value_type *value){
	for (auto iter = value_list_.begin(); iter != value_list_.end(); ++iter){
		if (iter->second.get() == value)
			return iter;
	}

	return value_list_.end();
}
