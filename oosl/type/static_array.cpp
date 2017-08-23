#include "static_array.h"

oosl::type::static_array::static_array(ptr_type underlying_type, size_type count)
	: array_type(underlying_type), count_(count){}

oosl::type::static_array::~static_array() = default;

oosl::type::object::driver_type *oosl::type::static_array::driver(){
	return nullptr;//#TODO: Implement
}

std::string oosl::type::static_array::name(){
	return (underlying_type_->name() + "[" + std::to_string(count_) + "]");
}

std::string oosl::type::static_array::print(){
	return (underlying_type_->print() + "[" + std::to_string(count_) + "]");
}

oosl::type::object::size_type oosl::type::static_array::size(){
	return (underlying_type_->size() * count_);
}

bool oosl::type::static_array::is_dynamic(){
	return false;
}

bool oosl::type::static_array::is_static_array(){
	return true;
}

oosl::type::object::size_type oosl::type::static_array::count(){
	return count_;
}
