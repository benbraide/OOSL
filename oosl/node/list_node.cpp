#include "list_node.h"

oosl::node::list::list(ptr_list_type &&list, const std::string &delimiter)
	: list_(std::forward<ptr_list_type>(list)), delimiter_(delimiter){}

oosl::node::list::~list() = default;

oosl::node::object::id_type oosl::node::list::id(){
	return id_type::list;
}

void oosl::node::list::echo(output_writer_type &writer){
	auto first_entry = true;
	for (auto item : list_){
		if (first_entry){//No delimiter
			first_entry = false;
			item->echo(writer);
		}
		else{//Print with delimiter
			writer.write(delimiter_);
			item->echo(writer);
		}
	}
}

void oosl::node::list::traverse(traverser_type traverser){
	for (auto item : list_){
		if (!traverser(item))
			break;//Canceled
	}
}
