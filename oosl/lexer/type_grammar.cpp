#include "type_grammar.h"

oosl::lexer::type_specifier_grammar::type_specifier_grammar()
	: type_specifier_grammar::base_type(start_, "OOSL_TYPE_SPECIFIER"){
	using namespace boost::spirit;

	start_ = (static_symbols_ >> thread_local_symbols_) | (thread_local_symbols_ > static_symbols_) | static_symbols_;

	static_symbols_.add
		("static", type_specifier::static_);

	thread_local_symbols_.add
		("thread_local", type_specifier::thread_local_);
}

oosl::lexer::primitive_type_grammar::primitive_type_grammar()
	: primitive_type_grammar::base_type(start_, "OOSL_PRIMITIVE_TYPE"){
	using namespace boost::spirit;

	start_ = type_symbols_;

	type_symbols_.add
		("void", primitive_type::void_)
		("any", primitive_type::any_)
		("bool", primitive_type::bool_)
		("byte", primitive_type::byte_)
		("char", primitive_type::char_)
		("wchar", primitive_type::wchar_)
		("int8", primitive_type::int8_)
		("uint8", primitive_type::uint8_)
		("int16", primitive_type::int16_)
		("uint16", primitive_type::uint16_)
		("int32", primitive_type::int32_)
		("uint32", primitive_type::uint32_)
		("int64", primitive_type::int64_)
		("uint64", primitive_type::uint64_)
		("int128", primitive_type::int128_)
		("uint128", primitive_type::uint128_)
		("float", primitive_type::float_)
		("double", primitive_type::double_)
		("double128", primitive_type::ldouble)
		("string", primitive_type::string_)
		("wstring", primitive_type::wstring_)
		("nullptr_t", primitive_type::nullptr_);
}
