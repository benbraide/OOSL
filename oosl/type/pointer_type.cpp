#include "pointer_type.h"

oosl::type::pointer::pointer(ptr_type underlying_type)
	: primitive(id_type::pointer_), underlying_type_(underlying_type){}

oosl::type::pointer::~pointer() = default;

oosl::type::object::driver_type *oosl::type::pointer::driver(){
	return nullptr;//#TODO: Implement
}

oosl::type::object *oosl::type::pointer::underlying_type(){
	return underlying_type_.get();
}

std::string oosl::type::pointer::name(){
	return print_(underlying_type_->name());
}

std::string oosl::type::pointer::print(){
	return print_(underlying_type_->print());
}

int oosl::type::pointer::score(object &type){
	auto value = object::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (type.is_strong_pointer())//Compare underlying types
		return ((underlying_type_->score(*type.underlying_type()) < (OOSL_MAX_TYPE_SCORE - 1)) ? OOSL_MIN_TYPE_SCORE : OOSL_MAX_TYPE_SCORE);

	if (type.is_pointer())
		return (OOSL_MAX_TYPE_SCORE - 1);

	if (type.is_strong_array())//Compare underlying types
		return ((underlying_type_->score(*type.underlying_type()) < (OOSL_MAX_TYPE_SCORE - 1)) ? OOSL_MIN_TYPE_SCORE : (OOSL_MAX_TYPE_SCORE - 2));

	return OOSL_MIN_TYPE_SCORE;
}

bool oosl::type::pointer::is_dynamic(){
	return false;
}

bool oosl::type::pointer::is_pointer(){
	return true;
}

bool oosl::type::pointer::is_strong_pointer(){
	return true;
}

std::string oosl::type::pointer::print_(const std::string &value){
	return (underlying_type_->is_strong_pointer() ? (value + "*") : (value + " *"));
}
