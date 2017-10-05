#include "type_grammar.h"

oosl::lexer::type_specifier_grammar::type_specifier_grammar()
	: base_type("OOSL_TYPE_SPECIFIER"){
	using namespace boost::spirit;

	start_ = 
		  (static_symbols_ >> thread_local_symbols_)	[qi::_val = boost::phoenix::bind(&combine, qi::_1, qi::_2)]
		| (thread_local_symbols_ > static_symbols_)		[qi::_val = boost::phoenix::bind(&combine, qi::_1, qi::_2)]
		| static_symbols_								[qi::_val = qi::_1];

	static_symbols_.add
		("static", type_attribute_type::static_);

	thread_local_symbols_.add
		("thread_local", type_attribute_type::tls);
}

oosl::lexer::type_specifier_grammar::type_attribute_type oosl::lexer::type_specifier_grammar::combine(type_attribute_type left, type_attribute_type right){
	return (left | right);
}

oosl::lexer::primitive_type_grammar::primitive_type_grammar()
	: grammar("OOSL_PRIMITIVE_TYPE"){
	using namespace boost::spirit;

	start_ = type_symbols_[qi::_val = boost::phoenix::bind(&create, qi::_1)];

	type_symbols_.add
		("void", type_id_type::void_)
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
	typedef oosl::node::inplace<type_ptr_type> node_type;

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
