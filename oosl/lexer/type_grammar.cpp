#include "type_grammar.h"
#include "expression_grammar.h"

oosl::lexer::storage_specifier_grammar::storage_specifier_grammar()
	: base_type("OOSL_TYPE_SPECIFIER"){
	using namespace boost::spirit;

	static_rule_ %= keyword(static_symbols_);
	thread_local_rule_ %= keyword(thread_local_symbols_);

	start_ =
		(static_rule_ >> thread_local_rule_)[qi::_val = boost::phoenix::bind(&combine, qi::_1, qi::_2)]
		| (thread_local_rule_ > static_rule_)[qi::_val = boost::phoenix::bind(&combine, qi::_1, qi::_2)]
		| static_rule_[qi::_val = qi::_1];

	static_symbols_.add
	("static", type_attribute_type::static_);

	thread_local_symbols_.add
	("thread_local", type_attribute_type::tls);
}

oosl::lexer::storage_specifier_grammar::type_attribute_type oosl::lexer::storage_specifier_grammar::combine(type_attribute_type left, type_attribute_type right){
	return (left | right);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::storage_specifier_grammar::create(type_attribute_type attributes, node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [attributes](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write(print_attributes(attributes));
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			auto modified_type = dynamic_cast<oosl::type::modified *>(underlying_type);

			if (modified_type == nullptr){//Add attributes
				OOSL_SET(modified_type->attributes_ref(), attributes);
				value.second = underlying_type->reflect();
			}
			else//Create
				value.second = std::make_shared<oosl::type::modified>(underlying_type->reflect(), attributes);
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}

std::string oosl::lexer::storage_specifier_grammar::print_attributes(type_attribute_type value){
	std::string string_value;
	if (OOSL_IS(value, type_attribute_type::static_))
		string_value = "static ";

	if (OOSL_IS(value, type_attribute_type::tls))
		string_value += "thread_local ";

	return string_value;
}

oosl::lexer::primitive_type_helper::node_ptr_type oosl::lexer::primitive_type_helper::create(type_id_type value){
	typedef oosl::storage::entry entry_type;
	typedef oosl::common::output_writer output_writer_type;
	typedef oosl::type::object type_object_type;

	typedef oosl::node::id node_id_type;
	typedef oosl::node::inplace_target_type inplace_target_type;
	typedef oosl::node::inplace<type_object_type::ptr_type> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.value());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write(owner.value()->name());
			return true;
		case inplace_target_type::type:
			*reinterpret_cast<type_object_type **>(out) = owner.value().get();
			return true;
		default:
			break;
		}

		return false;
	}, oosl::common::controller::active->find_type(value));
}

const char *oosl::lexer::primitive_type_helper::other_value(type_id_type id){
	switch (id){
	case type_id_type::auto_:
		return "auto";
	case type_id_type::void_:
		return "void";
	case type_id_type::pointer_:
		return "pointer_t";
	case type_id_type::array_:
		return "array_t";
	case type_id_type::function_:
		return "function_t";
	default:
		break;
	}

	return "";
}

oosl::lexer::primitive_type_grammar::primitive_type_grammar()
	: grammar("OOSL_PRIMITIVE_TYPE"){
	using namespace boost::spirit;

	start_ = keyword(type_symbols_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];

	type_symbols_.add
		("any", type_id_type::any_)
		("bool", type_id_type::bool_)
		("byte", type_id_type::byte_)
		("char", type_id_type::char_)
		("wchar", type_id_type::wchar_)
		("int8", type_id_type::int8_)
		("uint8", type_id_type::uint8_)
		("int16", type_id_type::int16_)
		("uint16", type_id_type::uint16_)
		("int32", type_id_type::int32_)
		("uint32", type_id_type::uint32_)
		("int64", type_id_type::int64_)
		("uint64", type_id_type::uint64_)
		("int128", type_id_type::int128_)
		("uint128", type_id_type::uint128_)
		("float32", type_id_type::float_)
		("float64", type_id_type::double_)
		("float128", type_id_type::ldouble)
		("string", type_id_type::string_)
		("wstring", type_id_type::wstring_)
		("nullptr_t", type_id_type::nullptr_)
		("type_t", type_id_type::type_)
		("pointer_t", type_id_type::pointer_)
		("array_t", type_id_type::array_)
		("function_t", type_id_type::function_)
		("node_t", type_id_type::node_)
		("storage_t", type_id_type::storage_)
		("nan_t", type_id_type::nan_);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::primitive_type_grammar::create(type_id_type value){
	return primitive_type_helper::create(value);
}

oosl::lexer::decltype_grammar::decltype_grammar()
	: grammar("OOSL_DECLTYPE"){
	using namespace boost::spirit;

	expression_ = std::make_shared<expression_grammar>();

	start_ = ("decltype" > qi::lit("(") > (*expression_) > ')')[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::decltype_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.type());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("decltype(");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write(")");
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto value = owner.value()->evaluate();//Evaluate expression
		*reinterpret_cast<type_object_type **>(out) = value->type->driver()->type(*value);

		return true;
	}, value);
}

oosl::lexer::function_type_grammar::function_type_grammar()
	: grammar("OOSL_FUNCTION_TYPE"){
	using namespace boost::spirit;

	return_type_ = std::make_shared<return_type_grammar>();
	parameter_type_ = std::make_shared<parameter_type_grammar>();
	variadic_type_ = std::make_shared<variadic_type_grammar>();

	type_ %= (((*parameter_type_) % ",") >> -(*variadic_type_));
	typed_ = ('<' > (void_type_ | (*return_type_)) > '(' > -type_ > qi::lit(')') > '>')
		[qi::_val = boost::phoenix::bind(&create_typed, qi::_1, qi::_2)];

	start_ = (keyword("function_t") >> -typed_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::function_type_grammar::create_typed(node_ptr_type return_type, const boost::optional<type_info> &info){
	typedef oosl::node::inplace<type_ptr_type> node_type;

	return std::make_shared<node_type>(node_id_type::type, [=](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.type());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("function_t<");
			return_type->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write("(");

			reinterpret_cast<output_writer_type *>(out)->write(")>");
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value == nullptr){//Evaluate
			
		}

		*reinterpret_cast<type_object_type **>(out) = value.get();
		return true;
	}, nullptr);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::function_type_grammar::create(boost::optional<node_ptr_type> typed){
	return (typed.is_initialized() ? typed.value() : primitive_type_helper::create(type_id_type::function_));
}

oosl::lexer::type_grammar::type_grammar()
	: grammar("OOSL_TYPE"){
	start_ %= (primitive_type_ | decltype_ | pointer_type_ | array_type_ | function_type_);
}

oosl::lexer::ref_type_grammar::ref_type_grammar()
	: grammar("OOSL_REF_TYPE"){
	using namespace boost::spirit;

	start_ = (keyword("ref") >> (type_ | identifier_compatible_))[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::ref_type_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.type());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("ref ");
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			value.second = std::make_shared<oosl::type::modified>(underlying_type->reflect(), type_object_type::attribute::ref);
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}

oosl::lexer::rval_ref_type_grammar::rval_ref_type_grammar()
	: grammar("OOSL_RVAL_REF_TYPE"){
	using namespace boost::spirit;

	start_ = (keyword("ref") >> keyword("val") >> (type_ | identifier_compatible_))[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::rval_ref_type_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("ref val ");
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			value.second = std::make_shared<oosl::type::modified>(underlying_type->reflect(), type_object_type::attribute::ref | type_object_type::attribute::rval);
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}

oosl::lexer::ref_auto_type_grammar::ref_auto_type_grammar()
	: grammar("OOSL_REF_TYPE"){
	using namespace boost::spirit;

	start_ = (keyword("ref") >> type_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::ref_auto_type_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*owner.type());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("ref ");
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			value.second = std::make_shared<oosl::type::modified>(underlying_type->reflect(), type_object_type::attribute::ref);
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}

oosl::lexer::rval_ref_auto_type_grammar::rval_ref_auto_type_grammar()
	: grammar("OOSL_RVAL_REF_TYPE"){
	using namespace boost::spirit;

	start_ = (keyword("ref") >> keyword("val") >> type_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::rval_ref_auto_type_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("ref val ");
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			value.second = std::make_shared<oosl::type::modified>(underlying_type->reflect(), type_object_type::attribute::ref | type_object_type::attribute::rval);
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}

oosl::lexer::return_type_grammar::return_type_grammar()
	: grammar("OOSL_RETURN_TYPE"){
	start_ %= (ref_auto_type_ | auto_type_ | ref_type_ | type_ | identifier_compatible_);
}

oosl::lexer::parameter_type_grammar::parameter_type_grammar()
	: grammar("OOSL_PARAMETER_TYPE"){
	start_ %= (rval_ref_auto_type_ | rval_ref_type_ | return_type_);
}

oosl::lexer::variadic_type_grammar::variadic_type_grammar()
	: grammar("OOSL_VARIADIC_TYPE"){
	using namespace boost::spirit;

	start_ = (parameter_type_ >> operator_("..."))[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::variadic_type_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<std::pair<node_ptr_type, type_ptr_type>> node_type;

	return std::make_shared<node_type>(node_id_type::type, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			owner.value().first->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write("...");
			return true;
		case inplace_target_type::type:
			break;
		default:
			return false;
		}

		auto &value = owner.value();
		if (value.second == nullptr){//Evaluate
			auto underlying_type = value.first->type();
			//value.second = std::make_shared<oosl::type::variadic>(underlying_type->reflect());
		}

		*reinterpret_cast<type_object_type **>(out) = value.second.get();
		return true;
	}, std::make_pair(value, nullptr));
}
