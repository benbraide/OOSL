#include "../driver/driver_object.h"

oosl::type::object::ptr_type oosl::type::object::reflect(){
	return shared_from_this();
}

oosl::type::object::driver_type *oosl::type::object::driver(){
	throw error_type::not_implemented;
}

oosl::type::object *oosl::type::object::underlying_type(){
	return nullptr;
}

oosl::type::object *oosl::type::object::non_modified(){
	return this;
}

oosl::type::object *oosl::type::object::non_variadic(){
	throw error_type::not_implemented;
}

std::string oosl::type::object::name(){
	throw error_type::not_implemented;
}

std::string oosl::type::object::print(){
	throw error_type::not_implemented;
}

oosl::type::object::size_type oosl::type::object::size(){
	return 0u;
}

int oosl::type::object::score(object &type){
	if (&type == this)//Exact types
		return OOSL_MAX_TYPE_SCORE;

	if (is_primitive() && type.is_primitive() && id() == type.id())
		return OOSL_MAX_TYPE_SCORE;//Exact IDs

	return (type.is_any() ? (OOSL_MAX_TYPE_SCORE - 1) : OOSL_MIN_TYPE_SCORE);
}

int oosl::type::object::score(storage_entry_type &entry){
	return score(*controller_type::active->driver(entry).type(entry)->non_modified());
}

oosl::type::object *oosl::type::object::match(object &type, match_type criteria){
	switch (criteria){
	case match_type::same:
		return (score(type) >= OOSL_MAX_TYPE_SCORE) ? this : nullptr;
	case match_type::compatible:
		return (score(type) > OOSL_MIN_TYPE_SCORE) ? this : nullptr;
	default:
		break;
	}

	return nullptr;
}

oosl::type::object *oosl::type::object::match(storage_entry_type &entry, match_type criteria){
	switch (criteria){
	case match_type::same:
		return (score(entry) >= OOSL_MAX_TYPE_SCORE) ? this : nullptr;
	case match_type::compatible:
		return (score(entry) > OOSL_MIN_TYPE_SCORE) ? this : nullptr;
	default:
		break;
	}

	return nullptr;
}

oosl::type::object::attribute oosl::type::object::attributes(){
	return attribute::nil;
}

oosl::type::object::id_type oosl::type::object::id(){
	return id_type::nil;
}

bool oosl::type::object::is(id_type id){
	return (this->id() == id);
}

bool oosl::type::object::is(attribute attributes){
	return OOSL_IS(this->attributes(), attributes);
}

bool oosl::type::object::is_any(attribute attributes){
	return OOSL_IS_ANY(this->attributes(), attributes);
}

bool oosl::type::object::is_same(object &type){
	return (score(type) >= OOSL_MAX_TYPE_SCORE);
}

bool oosl::type::object::is_compatible(object &type){
	return (score(type) > OOSL_MIN_TYPE_SCORE);
}

bool oosl::type::object::is_compatible(storage::entry &entry){
	return (score(entry) > OOSL_MIN_TYPE_SCORE);
}

bool oosl::type::object::is_class_compatible(object &type){
	throw error_type::not_implemented;
}

bool oosl::type::object::is_variant(){
	return is(id_type::variant_);
}

bool oosl::type::object::is_void(){
	return is(id_type::void_);
}

bool oosl::type::object::is_any(){
	return is(id_type::any_);
}

bool oosl::type::object::is_auto(){
	return is(id_type::auto_);
}

bool oosl::type::object::is_variadic(){
	return false;
}

bool oosl::type::object::is_enum(){
	return is(id_type::enum_);
}

bool oosl::type::object::is_union(){
	return is(id_type::union_);
}

bool oosl::type::object::is_struct(){
	return is(id_type::struct_);
}

bool oosl::type::object::is_class(){
	return is(id_type::class_);
}

bool oosl::type::object::is_primitive(){
	switch (id()){
	case id_type::union_:
	case id_type::struct_:
	case id_type::class_:
		return false;
	default:
		break;
	}

	return true;
}

bool oosl::type::object::is_dynamic(){
	switch (id()){
	case id_type::any_:
	case id_type::pointer_:
	case id_type::array_:
	case id_type::function_:
		return true;
	default:
		break;
	}

	return false;
}

bool oosl::type::object::is_numeric(){
	switch (id()){
	case id_type::char_:
	case id_type::wchar_:
	case id_type::int8_:
	case id_type::uint8_:
	case id_type::int16_:
	case id_type::uint16_:
	case id_type::int32_:
	case id_type::uint32_:
	case id_type::int64_:
	case id_type::uint64_:
	case id_type::int128_:
	case id_type::uint128_:
	case id_type::float_:
	case id_type::double_:
	case id_type::ldouble:
		return true;
	default:
		break;
	}

	return false;
}

bool oosl::type::object::is_integral(){
	switch (id()){
	case id_type::char_:
	case id_type::wchar_:
	case id_type::int8_:
	case id_type::uint8_:
	case id_type::int16_:
	case id_type::uint16_:
	case id_type::int32_:
	case id_type::uint32_:
	case id_type::int64_:
	case id_type::uint64_:
	case id_type::int128_:
	case id_type::uint128_:
		return true;
	default:
		break;
	}

	return false;
}

bool oosl::type::object::is_unsigned_integral(){
	switch (id()){
	case id_type::uint8_:
	case id_type::uint16_:
	case id_type::uint32_:
	case id_type::uint64_:
	case id_type::uint128_:
		return true;
	default:
		break;
	}

	return false;
}

bool oosl::type::object::is_floating_point(){
	switch (id()){
	case id_type::float_:
	case id_type::double_:
	case id_type::ldouble:
		return true;
	default:
		break;
	}

	return false;
}

bool oosl::type::object::is_pointer(){
	return (is(id_type::pointer_) || is(id_type::nullptr_));
}

bool oosl::type::object::is_strong_pointer(){
	return false;
}

bool oosl::type::object::is_string(){
	return is(id_type::string_);
}

bool oosl::type::object::is_wstring(){
	return is(id_type::wstring_);
}

bool oosl::type::object::is_array(){
	return is(id_type::array_);
}

bool oosl::type::object::is_strong_array(){
	return false;
}

bool oosl::type::object::is_static_array(){
	return false;
}

bool oosl::type::object::is_function(){
	return is(id_type::function_);
}

bool oosl::type::object::is_strong_function(){
	return false;
}

bool oosl::type::object::is_nullptr(){
	return is(id_type::nullptr_);
}

bool oosl::type::object::is_nan(){
	return is(id_type::nan_);
}

bool oosl::type::object::is_ref(){
	return false;
}

bool oosl::type::object::is_rval_ref(){
	return false;
}

bool oosl::type::object::is_specific(){
	return false;
}

bool oosl::type::object::is_static(){
	return is(attribute::static_);
}

bool oosl::type::object::is_thread_local(){
	return is(attribute::tls);
}

bool oosl::type::object::is_explicit(){
	return is(attribute::explicit_);
}

bool oosl::type::object::is_modified(){
	return (attributes() != attribute::nil);
}
