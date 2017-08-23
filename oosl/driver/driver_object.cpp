#include "driver_object.h"

oosl::driver::object::~object() = default;

oosl::driver::object::entry_type *oosl::driver::object::cast(entry_type &entry, type_object_type &type, cast_option_type options){
	auto is_rval_ref = (OOSL_IS(options, cast_option_type::val) || type.is_rval_ref());
	auto is_ref = (OOSL_IS(options, cast_option_type::ref) || type.is_ref()), is_lval = this->is_lval(entry);

	if (is_ref){
		if (!is_lval && !is_rval_ref)
			throw error_type::rval_to_lval;

		if (type.is_any() || type.non_modified()->is_same(*this->type(entry)->non_modified()))
			return &entry;

		if (!is_rval_ref)//Types must be the same
			throw error_type::incompatible_types;
	}

	if (is_uninitialized(entry)){//Only reference cast is permitted
		if (!is_ref || !is_lval)
			throw error_type::uninitialized_object;

		if (type.is_any() || type.non_modified()->is_same(*this->type(entry)->non_modified()))
			return &entry;

		throw error_type::uninitialized_object;
	}

	if (OOSL_IS(options, cast_option_type::reinterpret))
		throw error_type::not_implemented;//Cannot perform reinterpret cast

	if (!type.is_any() && !type.non_modified()->is_same(*this->type(entry)->non_modified()))
		throw error_type::incompatible_types;

	if (OOSL_IS(options, cast_option_type::make_copy)){

	}

	return &entry;
}

oosl::driver::object::type_object_type *oosl::driver::object::type(entry_type &entry){
	return entry.type.get();
}

oosl::driver::object::size_type oosl::driver::object::size(entry_type &entry){
	return entry.type->size();
}

oosl::driver::object::uint64_type oosl::driver::object::address(entry_type &entry){
	return entry.address;
}

oosl::driver::object::entry_type *oosl::driver::object::linked(entry_type &entry){
	throw error_type::not_implemented;
}

oosl::driver::object::attribute_type oosl::driver::object::attributes(entry_type &entry){
	auto attributes = entry.attributes;
	if (entry.type->is_ref())
		OOSL_SET(attributes, attribute_type::lval | attribute_type::ref_);

	if (entry.type->is_thread_local())
		OOSL_SET(attributes, attribute_type::tls);

	if (entry.type->is_nan())
		OOSL_SET(attributes, attribute_type::nan_);

	if (entry.type->is_void())
		OOSL_SET(attributes, attribute_type::void_);

	return attributes;
}

bool oosl::driver::object::is_lval(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::lval);
}

bool oosl::driver::object::is_ref(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::ref_);
}

bool oosl::driver::object::is_uninitialized(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::uninitialized);
}

bool oosl::driver::object::is_tls(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::tls);
}

bool oosl::driver::object::is_nan(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::nan_);
}

bool oosl::driver::object::is_void(entry_type &entry){
	return OOSL_IS(attributes(entry), attribute_type::void_);
}

void oosl::driver::object::value(entry_type &entry, type_id_type to, char *destination){
	throw error_type::not_implemented;
}
