#include "numeric_type.h"

oosl::type::numeric::numeric(id_type id)
	: primitive(id){}

oosl::type::numeric::~numeric() = default;

int oosl::type::numeric::score(object &type){
	auto value = primitive::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (!type.is_numeric())//Non-numeric type
		return ((type.is_string() || type.is_wstring()) ? (OOSL_MAX_TYPE_SCORE - 2) : OOSL_MIN_TYPE_SCORE);

	if (is_nan() || type.is_nan())//NaN comparison
		return (OOSL_MAX_TYPE_SCORE - 2);

	if (id_ < type.id())//Narrowing conversion
		return ((OOSL_MAX_TYPE_SCORE - 3) - (static_cast<int>(type.id()) - static_cast<int>(id_)));

	return (OOSL_MAX_TYPE_SCORE - 3);//Widening conversion
}

bool oosl::type::numeric::is_numeric(){
	return true;
}
