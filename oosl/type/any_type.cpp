#include "any_type.h"

oosl::type::any::any()
	: dynamic(id_type::any_){}

oosl::type::any::~any() = default;

int oosl::type::any::score(object &type){
	return (type.is_any() ? OOSL_MAX_TYPE_SCORE : (OOSL_MAX_TYPE_SCORE - 1));
}

bool oosl::type::any::is_any(){
	return true;
}
