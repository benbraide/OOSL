#include "indexed_node.h"

oosl::node::indexed::indexed(const index_type &index)
	: index_(index){}

oosl::node::indexed::~indexed() = default;

oosl::node::object::index_type &oosl::node::indexed::index(){
	return index_;
}
