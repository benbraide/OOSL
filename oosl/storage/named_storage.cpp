#include "named_storage.h"

oosl::storage::named::named(const std::string &name, object *parent)
	: object(parent), name_(name){}

oosl::storage::named::~named() = default;

oosl::storage::object *oosl::storage::named::match(const std::string &name){
	return ((name == name_) ? this : object::match(name));
}

const std::string &oosl::storage::named::name() const{
	return name_;
}

std::string oosl::storage::named::print() const{
	auto named_parent = dynamic_cast<named *>(parent_);
	return ((named_parent == nullptr) ? name_ : (named_parent->print() + "::" + name_));
}
