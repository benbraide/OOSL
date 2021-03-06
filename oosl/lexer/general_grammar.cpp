#include "general_grammar.h"
#include "expression_grammar.h"

oosl::lexer::expression_list_grammar::expression_list_grammar()
	: grammar("OOSL_EXPRESSION_LIST"){
	using namespace boost::spirit;

	expression_ = std::make_shared<non_list_expression_grammar>();
	variadic_expression_ = std::make_shared<variadic_expression_grammar>();

	start_ = (((*expression_) | (*variadic_expression_)) % ",")[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::expression_list_grammar::create(const node_ptr_list_type &value){
	typedef oosl::node::inplace<void> inplace_type;
	typedef oosl::node::object::traverser_type traverser_type;

	return std::make_shared<inplace_type>(node_id_type::list, [value](inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			break;
		case inplace_target_type::traverse:
		{
			auto &traverser = *reinterpret_cast<traverser_type *>(out);
			for (auto item : value){
				if (!traverser(item))
					break;
			}
			return true;
		}
		default:
			return false;
		}

		auto first_print = true;
		auto &writer = *reinterpret_cast<output_writer_type *>(out);

		for (auto item : value){
			if (first_print)
				first_print = false;
			else//Put delimiter
				reinterpret_cast<output_writer_type *>(out)->write(", ");
			item->echo(writer);
		}

		return true;
	});
}

oosl::lexer::keyword_grammar::keyword_grammar()
	: grammar("OOSL_KEYWORD"){
	using namespace boost::spirit;

	start_ = keyword(symbols_)[qi::_val = create()];//(symbols_ >> !qi::char_("$_A-Za-z0-9"));

	symbols_.add
		("module")
		("export")
		("import")
		("void")
		("any")
		("auto")
		("bool")
		("byte")
		("char")
		("wchar")
		("int8")
		("uint8")
		("int16")
		("uint16")
		("int32")
		("uint32")
		("int64")
		("uint64")
		("int128")
		("uint128")
		("float32")
		("float64")
		("float128")
		("string")
		("wstring")
		("enum")
		("union")
		("struct")
		("class")
		("namespace")
		("ref")
		("array_t")
		("pointer_t")
		("function_t")
		("nullptr_t")
		("nan_t")
		("type_t")
		("node_t")
		("storage_t")
		("decltype")
		("static_cast")
		("reinterpret_cast")
		("static")
		("thread_local")
		("true")
		("false")
		("indeterminate")
		("nullptr")
		("NaN")
		("using")
		("typedef")
		("operator")
		("sizeof")
		("typeof")
		("echo")
		("if")
		("else")
		("unless")
		("while")
		("do")
		("until")
		("for")
		("switch")
		("case")
		("default")
		("break")
		("continue")
		("return")
		("throw")
		("try")
		("catch")
		("finally")
		("new")
		("delete")
		("self")
		("__placeholder")
		("__call");
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::keyword_grammar::create(){
	return nullptr;
}

oosl::lexer::identifier_grammar::identifier_grammar()
	: grammar("OOSL_IDENITIFIER"){
	using namespace boost::spirit;

	base_ = qi::raw[qi::lexeme[qi::char_("$_A-Za-z") >> *qi::char_("$_A-Za-z0-9")]];
	start_ = (!keyword_ >> base_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::identifier_grammar::create(const std::string &value){
	typedef oosl::node::inplace<std::string> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::identifier, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = lookup(owner.value());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write(owner.value());
			return true;
		case inplace_target_type::key:
			*reinterpret_cast<std::string *>(out) = owner.value();
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::grammar::entry_type *oosl::lexer::identifier_grammar::lookup(const std::string &key){
	auto &runtime_info = oosl::common::controller::active->runtime_info();
	auto value = runtime_info.storage->find(key, runtime_info.find_type);
	if (value != nullptr){
		auto object_value = value->object();
		if (object_value == nullptr){//Not an object
			auto type_value = value->type();
			if (type_value == nullptr)//Return storage object
				return oosl::common::controller::active->temporary_storage().add_scalar(*value->storage());

			return oosl::common::controller::active->temporary_storage().add_scalar(*type_value);//Return type object
		}

		return object_value;//Is object
	}

	throw oosl::common::error_codes::not_found;//Not found
}

oosl::lexer::placeholder_grammar::placeholder_grammar()
	: grammar("OOSL_PLACEHOLDER"){
	using namespace boost::spirit;

	expression_ = std::make_shared<expression_grammar>();

	start_ = ("__placeholder" > qi::lit("(") > (*expression_) > ")")[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::placeholder_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::placeholder, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = identifier_grammar::lookup(owner.key());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("__placeholder(");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write(")");
			return true;
		case inplace_target_type::key:
		{//Evaluate value and return string value
			auto value = owner.value()->evaluate();
			*reinterpret_cast<std::string *>(out) = value->type->driver()->value<std::string>(*value);
			return true;
		}
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::identifier_or_placeholder_grammar::identifier_or_placeholder_grammar()
	: grammar("OOSL_IDENITIFIER_OR_PLACEHOLDER"){
	using namespace boost::spirit;
	start_ %= (placeholder_ | identifier_);
}

oosl::lexer::absolute_qualified_grammar::absolute_qualified_grammar()
	: grammar("OOSL_ABSOLUTE_QUALIFIED"){
	using namespace boost::spirit;

	start_ = "::" >> identifier_[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::absolute_qualified_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::qualified, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
		{//Evaluate using global storage
			auto &runtime_info = oosl::common::controller::active->runtime_info();

			oosl::common::raii_state<oosl::storage::object *> storage_state(runtime_info.storage, &oosl::common::controller::active->global_storage());
			oosl::common::raii_state<oosl::storage::object::find_type> find_state(runtime_info.find_type, oosl::storage::object::find_type::nil);//Disable recursive search

			*reinterpret_cast<entry_type **>(out) = owner.value()->evaluate();
			return true;
		}
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("::");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::relative_qualified_grammar::relative_qualified_grammar()
	: grammar("OOSL_RELATIVE_QUALIFIED"){
	using namespace boost::spirit;

	start_ =
		(absolute_qualified_ | identifier_)[qi::_val = qi::_1]
		>> +("::" >> identifier_)[qi::_val = boost::phoenix::bind(&create, qi::_val, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::relative_qualified_grammar::create(node_ptr_type left, node_ptr_type right){
	typedef oosl::node::inplace<std::pair<node_ptr_type, node_ptr_type>> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::qualified, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
		{//Evaluate using specified storage
			auto &value = owner.value();
			auto storage = value.first->storage();
			if (storage == nullptr)//Not found
				throw oosl::common::error_codes::storage_not_found;

			auto &runtime_info = oosl::common::controller::active->runtime_info();
			oosl::common::raii_state<oosl::storage::object *> storage_state(storage, &oosl::common::controller::active->global_storage());
			oosl::common::raii_state<oosl::storage::object::find_type> find_state(runtime_info.find_type, oosl::storage::object::find_type::nil);//Disable recursive search

			*reinterpret_cast<entry_type **>(out) = value.second->evaluate();
			return true;
		}
		case inplace_target_type::print:
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write("::");
			owner.value().second->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		default:
			break;
		}

		return false;
	}, std::make_pair(left, right));
}

oosl::lexer::qualified_grammar::qualified_grammar()
	: grammar("OOSL_QUALIFIED"){
	using namespace boost::spirit;

	start_ %= (relative_qualified_ | absolute_qualified_);
}

oosl::lexer::identifier_compatible_grammar::identifier_compatible_grammar()
	: grammar("OOSL_FULL_IDENITIFIER_COMPATIBLE"){
	using namespace boost::spirit;

	start_ %= (qualified_ | identifier_);
}

oosl::lexer::system_call_grammar::system_call_grammar()
	: grammar("OOSL_SYSTEM_CALL"){
	using namespace boost::spirit;

	start_ = ("__call" >> qi::lit("(") >> qi::uint_ >> -(',' >> expression_list_) >> ')')[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::system_call_grammar::create(unsigned int value, boost::optional<node_ptr_type> args){
	typedef oosl::node::inplace<unsigned int> inplace_type;
	return std::make_shared<inplace_type>(node_id_type::placeholder, [args](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("__call(");
			reinterpret_cast<output_writer_type *>(out)->write(std::to_string(owner.value()));
			if (args.is_initialized()){//Print arguments
				reinterpret_cast<output_writer_type *>(out)->write(", ");
				args.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			}
			reinterpret_cast<output_writer_type *>(out)->write(")");
			return true;
		default:
			break;
		}

		return false;
	}, value);
}
