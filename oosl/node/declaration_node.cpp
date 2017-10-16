#include "declaration_node.h"
#include "../lexer/type_grammar.h"

oosl::node::declaration::declaration(storage_attribute_type attributes, ptr_type type, ptr_type id, ptr_type init)
	: attributes_(attributes), type_(type), id_(id), init_(init){}

oosl::node::declaration::~declaration(){
	if (!common::controller::active->exiting() && static_value_.address != 0u)
		common::controller::active->memory().deallocate(static_value_.address);
}

oosl::node::object::id_type oosl::node::declaration::id(){
	return id_type::declaration;
}

void oosl::node::declaration::echo(output_writer_type &writer){
	std::string storage_specifier = lexer::storage_specifier_grammar::print_attributes(attributes_);
	if (!storage_specifier.empty())//Print storage specifiers
		writer.write(storage_specifier);

	type_->echo(writer);
	if (id_ != nullptr){//Print ID
		writer.write(" ");
		id_->echo(writer);
	}
	
	if (init_ != nullptr){//Print initialization
		writer.write(" = ");
		init_->echo(writer);
	}
}

oosl::node::object::type_object_type *oosl::node::declaration::type(){
	return type_->type();
}

oosl::node::object::ptr_type oosl::node::declaration::type_node(){
	return type_;
}

oosl::node::object::entry_type *oosl::node::declaration::evaluate_(){
	auto &runtime_info = common::controller::active->runtime_info();
	if (static_value_.address != 0u)
		return runtime_info.storage->add(id_->key(), static_value_, reflect())->object();

	entry_type *init_value = nullptr;
	auto init = ((runtime_info.decl_init == nullptr) ? init_ : runtime_info.decl_init);

	if (init != nullptr)//Evaluate initialization
		init_value = init->evaluate();

	auto type_value = type_->type()->reflect();
	if (type_value->is_auto()){//Use initialization type
		if (init_value == nullptr)//Initialization required
			throw error_type::missing_initialization;

		auto init_type = init_value->type->driver()->type(*init_value)->reflect();
		if (type_value->is_ref())
			type_value = std::make_shared<oosl::type::modified>(init_type, storage_attribute_type::ref);
		else//Not a reference
			type_value = init_type;
	}

	auto memory_block = common::controller::active->memory().allocate(type_value->size(), runtime_info.address_offset);
	if (runtime_info.address_offset != 0u)//Add offset
		runtime_info.address_offset += memory_block->actual_size;

	auto attributes = entry_type::attribute_type::nil;
	if (OOSL_IS(attributes_, storage_attribute_type::static_)){//Static declaration
		OOSL_SET(attributes, entry_type::attribute_type::static_ | entry_type::attribute_type::no_free);
		if (OOSL_IS(attributes_, storage_attribute_type::tls))
			OOSL_SET(attributes, entry_type::attribute_type::tls);
	}

	entry_type entry{ runtime_info.storage, memory_block->address, attributes, type_value };
	type_value->driver()->assign(entry, *init_value);//Assign initialization value

	if (id_ == nullptr)//No identifier
		return common::controller::active->temporary_storage().wrap(memory_block->address, type_value, attributes);

	auto value = runtime_info.storage->add(id_->key(), entry, reflect());
	if (OOSL_IS(attributes_, storage_attribute_type::static_))
		static_value_ = entry;//Store static value

	return value->object();
}
