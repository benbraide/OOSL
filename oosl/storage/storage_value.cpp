#include "storage_object.h"
#include "../type/type_object.h"

oosl::storage::value::entry_type *oosl::storage::value::object(){
	return (is_object() ? &std::get<entry_type>(value_) : nullptr);
}

oosl::storage::value::type_object_type *oosl::storage::value::type(){
	return (is_type() ? std::get<type_ptr_type>(value_).get() : nullptr);
}

oosl::storage::value::object_type *oosl::storage::value::storage(){
	return (is_storage() ? std::get<object_ptr_type>(value_).get() : nullptr);
}

oosl::storage::value::uint64_type oosl::storage::value::unknown(){
	return (is_unknown() ? std::get<uint64_type>(value_) : 0u);
}

bool oosl::storage::value::is_object() const{
	return std::holds_alternative<entry_type>(value_);
}

bool oosl::storage::value::is_type() const{
	return std::holds_alternative<type_ptr_type>(value_);
}

bool oosl::storage::value::is_storage() const{
	return std::holds_alternative<object_ptr_type>(value_);
}

bool oosl::storage::value::is_unknown() const{
	return std::holds_alternative<uint64_type>(value_);
}
