#include "modified_type.h"

oosl::type::modified::modified(ptr_type underlying_type, attribute attributes)
	: underlying_type_(underlying_type), attributes_(attributes){}

oosl::type::modified::~modified() = default;

oosl::type::object::driver_type *oosl::type::modified::driver(){
	return ((is_ref() && !is_rval_ref()) ? controller_type::active->find_driver(driver_key_type::dynamic) : underlying_type_->driver());
}

oosl::type::object *oosl::type::modified::underlying_type(){
	return underlying_type_.get();
}

oosl::type::object *oosl::type::modified::non_modified(){
	return underlying_type_->non_modified();
}

oosl::type::object *oosl::type::modified::non_variadic(){
	return underlying_type_->non_variadic();
}

oosl::type::object &oosl::type::modified::function_return_type(){
	return underlying_type_->function_return_type();
}

oosl::type::object::ptr_list_type &oosl::type::modified::function_parameters(){
	return underlying_type_->function_parameters();
}

std::string oosl::type::modified::name(){
	return underlying_type_->name();
}

std::string oosl::type::modified::print(){
	std::string prefix;
	if (is_static())
		prefix = "static ";

	if (is_thread_local())
		prefix += "thread_local ";

	std::string suffix;
	if (is_rval_ref()){
		if (underlying_type_->is_strong_pointer())
			suffix = "&&";
		else
			suffix = " &&";
	}
	else if (is_ref()){
		if (underlying_type_->is_strong_pointer())
			suffix = "&";
		else
			suffix = " &";
	}

	return (prefix + underlying_type_->print() + suffix);
}

oosl::type::object::size_type oosl::type::modified::size(){
	return ((is_ref() && !is_rval_ref()) ? static_cast<size_type>(sizeof(uint64_type)) : underlying_type_->size());
}

int oosl::type::modified::score(object &type){
	if (is_rval_ref() != type.is_rval_ref() || is_ref() != type.is_ref())
		return OOSL_MIN_TYPE_SCORE;
	return underlying_type_->score(*type.non_modified());
}

int oosl::type::modified::score(storage_entry_type &entry){
	return underlying_type_->score(entry);
}

int oosl::type::modified::score(const storage_entry_list_type &entries){
	return underlying_type_->score(entries);
}

oosl::type::object::attribute oosl::type::modified::attributes(){
	return attributes_;
}

oosl::type::object::id_type oosl::type::modified::id(){
	return underlying_type_->id();
}

bool oosl::type::modified::is(id_type id){
	return underlying_type_->is(id);
}

bool oosl::type::modified::is(attribute attributes){
	return OOSL_IS(attributes_, attributes);
}

bool oosl::type::modified::is_any(attribute attributes){
	return OOSL_IS_ANY(attributes_, attributes);
}

bool oosl::type::modified::is_variant(){
	return underlying_type_->is_variant();
}

bool oosl::type::modified::is_void(){
	return underlying_type_->is_void();
}

bool oosl::type::modified::is_any(){
	return underlying_type_->is_any();
}

bool oosl::type::modified::is_auto(){
	return underlying_type_->is_auto();
}

bool oosl::type::modified::is_variadic(){
	return underlying_type_->is_variadic();
}

bool oosl::type::modified::is_enum(){
	return underlying_type_->is_enum();
}

bool oosl::type::modified::is_union(){
	return underlying_type_->is_union();
}

bool oosl::type::modified::is_struct(){
	return underlying_type_->is_struct();
}

bool oosl::type::modified::is_class(){
	return underlying_type_->is_class();
}

bool oosl::type::modified::is_primitive(){
	return underlying_type_->is_primitive();
}

bool oosl::type::modified::is_dynamic(){
	return underlying_type_->is_dynamic();
}

bool oosl::type::modified::is_numeric(){
	return underlying_type_->is_numeric();
}

bool oosl::type::modified::is_integral(){
	return underlying_type_->is_integral();
}

bool oosl::type::modified::is_unsigned_integral(){
	return underlying_type_->is_unsigned_integral();
}

bool oosl::type::modified::is_floating_point(){
	return underlying_type_->is_floating_point();
}

bool oosl::type::modified::is_pointer(){
	return underlying_type_->is_pointer();
}

bool oosl::type::modified::is_strong_pointer(){
	return underlying_type_->is_strong_pointer();
}

bool oosl::type::modified::is_string(){
	return underlying_type_->is_string();
}

bool oosl::type::modified::is_wstring(){
	return underlying_type_->is_wstring();
}

bool oosl::type::modified::is_array(){
	return underlying_type_->is_array();
}

bool oosl::type::modified::is_strong_array(){
	return underlying_type_->is_strong_array();
}

bool oosl::type::modified::is_static_array(){
	return underlying_type_->is_static_array();
}

bool oosl::type::modified::is_function(){
	return underlying_type_->is_function();
}

bool oosl::type::modified::is_strong_function(){
	return underlying_type_->is_strong_function();
}

bool oosl::type::modified::is_nullptr(){
	return underlying_type_->is_nullptr();
}

bool oosl::type::modified::is_nan(){
	return underlying_type_->is_nan();
}

bool oosl::type::modified::is_modified(){
	return underlying_type_->is_modified();
}
