#include "node_object.h"
#include "../common/structures.h"

oosl::node::object::~object() = default;

oosl::node::object::ptr_type oosl::node::object::reflect(){
	return shared_from_this();
}

oosl::node::object::id_type oosl::node::object::id(){
	throw error_type::not_implemented;
}

oosl::node::object::index_type &oosl::node::object::index(){
	throw error_type::not_implemented;
}

bool oosl::node::object::is(id_type id){
	return (id == this->id());
}

std::string oosl::node::object::print(){
	throw error_type::not_implemented;
}

std::string oosl::node::object::key(){
	throw error_type::not_implemented;
}

oosl::node::object::entry_type *oosl::node::object::evaluate(){
	common::raii_state<ptr_type> raii_colum(common::controller::active->runtime_info().column, reflect());
	return evaluate_();
}

oosl::node::object::storage_type *oosl::node::object::storage(){
	auto value = common::controller::active->runtime_info().storage->find(key(), storage_type::find_type::recursive);
	return ((value == nullptr) ? nullptr : value->storage());
}

oosl::node::object::type_object_type *oosl::node::object::type(){
	auto value = common::controller::active->runtime_info().storage->find(key(), storage_type::find_type::recursive);
	return ((value == nullptr) ? nullptr : value->type());
}

oosl::node::object::entry_type *oosl::node::object::evaluate_(){
	throw error_type::not_implemented;
}
