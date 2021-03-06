#include "node_object.h"
#include "../common/structures.h"

oosl::node::object::~object() = default;

oosl::node::object::ptr_type oosl::node::object::reflect(){
	return shared_from_this();
}

oosl::node::object::ptr_type oosl::node::object::substitute(const std::string &key, ptr_type value){
	return reflect();
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

void oosl::node::object::echo(){
	echo(common::controller::active->output_writer(output_writer_key_type::nil));
}

void oosl::node::object::echo(output_writer_type &writer){
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
	auto &run_info = common::controller::active->runtime_info();
	auto value = run_info.storage->find(key(), run_info.find_type);
	if (value == nullptr)//Not found
		return nullptr;

	auto storage_value = value->storage();
	return ((storage_value == nullptr) ? dynamic_cast<storage_type *>(value->type()) : storage_value);
}

oosl::node::object::type_object_type *oosl::node::object::type(){
	auto &run_info = common::controller::active->runtime_info();
	auto value = run_info.storage->find(key(), run_info.find_type);
	return ((value == nullptr) ? nullptr : value->type());
}

oosl::node::object::ptr_type oosl::node::object::type_node(){
	throw error_type::not_implemented;
}

oosl::node::object::size_type oosl::node::object::count(){
	return 1u;
}

void oosl::node::object::traverse(traverser_type traverser){}

oosl::node::object::entry_type *oosl::node::object::evaluate_(){
	throw error_type::not_implemented;
}
