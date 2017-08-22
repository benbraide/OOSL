#include "dynamic_type.h"

oosl::type::dynamic::dynamic(id_type id)
	: primitive(id){}

oosl::type::dynamic::~dynamic() = default;

oosl::type::object::driver_type *oosl::type::dynamic::driver(){
	return nullptr;//#TODO: Implement
}

int oosl::type::dynamic::score(object &type){
	auto value = primitive::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (is_pointer()){//Pointer comparison
		if (type.is_pointer())
			return (OOSL_MAX_TYPE_SCORE - 1);
		return (type.is_array() ? (OOSL_MAX_TYPE_SCORE - 2) : OOSL_MIN_TYPE_SCORE);
	}

	if (is_array())//Array comparison
		return (type.is_array() ? (OOSL_MAX_TYPE_SCORE - 1) : OOSL_MIN_TYPE_SCORE);

	if (is_function())//Function comparison
		return (type.is_function() ? (OOSL_MAX_TYPE_SCORE - 1) : OOSL_MIN_TYPE_SCORE);

	return OOSL_MIN_TYPE_SCORE;
}

bool oosl::type::dynamic::is_dynamic(){
	return true;
}
