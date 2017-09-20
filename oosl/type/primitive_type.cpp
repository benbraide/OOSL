#include "primitive_type.h"

oosl::type::primitive::primitive(id_type id)
	: id_(id){
	switch (id_){
	case id_type::void_:
		name_ = "void";
		size_ = static_cast<size_type>(0);
		break;
	case id_type::any_:
		name_ = "any";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::auto_:
		name_ = "auto";
		size_ = static_cast<size_type>(0);
		break;
	case id_type::bool_:
		name_ = "bool";
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::byte_:
		name_ = "byte";
		size_ = static_cast<size_type>(sizeof(unsigned char));
		break;
	case id_type::char_:
		name_ = "char";
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::wchar_:
		name_ = "wchar";
		size_ = static_cast<size_type>(sizeof(wchar_t));
		break;
	case id_type::int8_:
		name_ = "int8_t";
		size_ = static_cast<size_type>(sizeof(__int8));
		break;
	case id_type::uint8_:
		name_ = "uint8_t";
		size_ = static_cast<size_type>(sizeof(unsigned __int8));
		break;
	case id_type::int16_:
		name_ = "int16_t";
		size_ = static_cast<size_type>(sizeof(__int16));
		break;
	case id_type::uint16_:
		name_ = "uint16_t";
		size_ = static_cast<size_type>(sizeof(unsigned __int16));
		break;
	case id_type::int32_:
		name_ = "int32_t";
		size_ = static_cast<size_type>(sizeof(__int32));
		break;
	case id_type::uint32_:
		name_ = "uint32_t";
		size_ = static_cast<size_type>(sizeof(unsigned __int32));
		break;
	case id_type::int64_:
		name_ = "int64_t";
		size_ = static_cast<size_type>(sizeof(__int64));
		break;
	case id_type::uint64_:
		name_ = "uint64_t";
		size_ = static_cast<size_type>(sizeof(unsigned __int64));
		break;
	case id_type::int128_:
		name_ = "int128_t";
		size_ = static_cast<size_type>(16);
		break;
	case id_type::uint128_:
		name_ = "uint128_t";
		size_ = static_cast<size_type>(16);
		break;
	case id_type::float_:
		name_ = "float32_t";
		size_ = static_cast<size_type>(sizeof(float));
		break;
	case id_type::double_:
		name_ = "float64_t";
		size_ = static_cast<size_type>(sizeof(double));
		break;
	case id_type::ldouble:
		name_ = "float128_t";
		size_ = static_cast<size_type>(sizeof(long double));
		break;
	case id_type::nullptr_:
		name_ = "nullptr_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::type_:
		name_ = "type_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::pointer_:
		name_ = "pointer_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::array_:
		name_ = "array_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::function_:
		name_ = "function_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::node_:
		name_ = "node_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::storage_:
		name_ = "storage_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	case id_type::nan_:
		name_ = "nan_t";
		size_ = static_cast<size_type>(sizeof(uint64_type));
		break;
	default:
		size_ = static_cast<size_type>(0);
		break;
	}
}

oosl::type::primitive::~primitive() = default;

oosl::type::object::driver_type *oosl::type::primitive::driver(){
	switch (id_){
	case id_type::void_:
		break;
	case id_type::bool_:
		return controller_type::active->find_driver(driver_key_type::boolean);
	case id_type::byte_:
		return controller_type::active->find_driver(driver_key_type::byte);
	case id_type::char_:
	case id_type::wchar_:
		return controller_type::active->find_driver(driver_key_type::char_);
	case id_type::nullptr_:
		return controller_type::active->find_driver(driver_key_type::pointer);
	case id_type::type_:
		break;
	case id_type::node_:
		break;
	case id_type::storage_:
		break;
	case id_type::nan_:
		return controller_type::active->find_driver(driver_key_type::numeric);
	default:
		break;
	}

	throw error_type::driver_not_found;
}

std::string oosl::type::primitive::name(){
	return name_;
}

std::string oosl::type::primitive::print(){
	return name_;
}

oosl::type::object::size_type oosl::type::primitive::size(){
	return size_;
}

int oosl::type::primitive::score(object &type){
	auto value = object::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (is_any())
		return (OOSL_MAX_TYPE_SCORE - 1);

	return (type.is_primitive() && id_ == type.id()) ? OOSL_MAX_TYPE_SCORE : OOSL_MIN_TYPE_SCORE;
}

oosl::type::object::id_type oosl::type::primitive::id(){
	return id_;
}

bool oosl::type::primitive::is_primitive(){
	return true;
}
