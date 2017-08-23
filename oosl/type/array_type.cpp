#include "array_type.h"

oosl::type::array_type::array_type(ptr_type underlying_type)
	: dynamic(id_type::array_), underlying_type_(underlying_type){}

oosl::type::array_type::~array_type() = default;

oosl::type::object *oosl::type::array_type::underlying_type(){
	return underlying_type_.get();
}

std::string oosl::type::array_type::name(){
	return ("array_t<" + underlying_type_->name() + ">");
}

std::string oosl::type::array_type::print(){
	return ("array_t<" + underlying_type_->print() + ">");
}

int oosl::type::array_type::score(object &type){
	auto value = object::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (type.is_strong_array())//Compare underlying types
		return ((underlying_type_->score(*type.underlying_type()) < (OOSL_MAX_TYPE_SCORE - 1)) ? OOSL_MIN_TYPE_SCORE : OOSL_MAX_TYPE_SCORE);

	if (type.is_array())
		return (OOSL_MAX_TYPE_SCORE - 1);

	return OOSL_MIN_TYPE_SCORE;
}

bool oosl::type::array_type::is_array(){
	return true;
}

bool oosl::type::array_type::is_strong_array(){
	return true;
}
