#include "named_storage.h"

oosl::storage::named::named(const std::string &name, object *parent)
	: object(parent), name_(name), this_value_(this){}

oosl::storage::named::~named() = default;

oosl::storage::object *oosl::storage::named::match(const std::string &name){
	return ((name == name_) ? this : object::match(name));
}

oosl::storage::object::value_type *oosl::storage::named::find(const std::string &key, find_type type){
	return ((type == find_type::recursive && key == name_) ? &this_value_ : object::find(key, type));
}

const std::string &oosl::storage::named::name() const{
	return name_;
}

std::string oosl::storage::named::print() const{
	auto named_parent = dynamic_cast<named *>(parent_);
	return ((named_parent == nullptr) ? name_ : (named_parent->print() + "::" + name_));
}
