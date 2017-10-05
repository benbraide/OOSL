#include "literal_grammar.h"
#include "expression_grammar.h"

bool oosl::lexer::numeric_helper::pass_radix(int value){
	return ((value <= -2 && value >= -36) || (value >= 2 && value <= 36));
}

oosl::lexer::numeric_helper::node_ptr_type oosl::lexer::numeric_helper::make_rad_node(int base, const std::string &value, boost::optional<integral_suffix_type> suffix){
	if (pass_radix(base))
		throw oosl::common::error_codes::bad_integer;

	std::size_t index = 0;
	long long converted_value;

	auto is_negative = false;
	if (base < 0){
		is_negative = true;
		base = -base;
	}

	try{
		converted_value = std::stoll(value, &index, base);
		if (index < value.size())
			throw oosl::common::error_codes::bad_integer;
	}
	catch (...){
		throw oosl::common::error_codes::bad_integer;
	}

	return (is_negative ? make_int_node(-converted_value, suffix) : make_int_node(converted_value, suffix));
}

oosl::lexer::numeric_helper::node_ptr_type oosl::lexer::numeric_helper::make_int_node(long long value, boost::optional<integral_suffix_type> suffix){
	switch (suffix.value_or(integral_suffix_type::nil)){
	case integral_suffix_type::nil://No suffix
	case integral_suffix_type::i32:
		break;
	case integral_suffix_type::i8:
		return make_node<oosl::node::inplace<__int8>>(value, "i8");
	case integral_suffix_type::ui8:
		return make_node<oosl::node::inplace<unsigned __int8>>(value, "ui8");
	case integral_suffix_type::i16:
		return make_node<oosl::node::inplace<__int16>>(value, "i16");
	case integral_suffix_type::ui16:
		return make_node<oosl::node::inplace<unsigned __int16>>(value, "ui16");
	case integral_suffix_type::ui32:
		return make_node<oosl::node::inplace<unsigned __int32>>(value, "ui32");
	case integral_suffix_type::i64:
		return make_node<oosl::node::inplace<__int64>>(value, "i64");
	case integral_suffix_type::ui64:
		return make_node<oosl::node::inplace<unsigned __int64>>(value, "ui64");
	default:
		throw oosl::common::error_codes::invalid_suffix;
	}

	return make_node<oosl::node::inplace<__int32>>(value, "i32");
}

oosl::lexer::numeric_helper::node_ptr_type oosl::lexer::numeric_helper::make_real_node(long double value, boost::optional<real_suffix_type> suffix){
	switch (suffix.value_or(real_suffix_type::nil)){
	case real_suffix_type::nil://No suffix
	case real_suffix_type::f64://double
		break;
	case real_suffix_type::f32://float
		return make_node<oosl::node::inplace<float>>(value, "f32");
	case real_suffix_type::f128://long double
		return make_node<oosl::node::inplace<long double>>(value, "f128");
	default:
		throw oosl::common::error_codes::invalid_suffix;
	}

	return make_node<oosl::node::inplace<double>>(value, "f64");
}

oosl::lexer::rad_literal_grammar::rad_literal_grammar()
	: grammar("OOSL_RAD_LITERAL"){
	using namespace boost::spirit;

	start_ = qi::lexeme[(qi::int_ >> 'r' >> qi::as_string[+qi::alnum] >> -suffix_symbols_)[qi::_val = boost::phoenix::bind(&numeric_helper::make_rad_node, qi::_1, qi::_2, qi::_3)]];

	suffix_symbols_.add
		("i8", integral_suffix_type::i8)
		("ui8", integral_suffix_type::ui8)
		("i16", integral_suffix_type::i16)
		("ui16", integral_suffix_type::ui16)
		("i32", integral_suffix_type::i32)
		("ui32", integral_suffix_type::ui32)
		("i64", integral_suffix_type::i64)
		("ui64", integral_suffix_type::ui64)
		("i128", integral_suffix_type::i128)
		("ui128", integral_suffix_type::ui128);
}

oosl::lexer::integer_literal_grammar::integer_literal_grammar()
	: grammar("OOSL_INTEGER_LITERAL"){
	using namespace boost::spirit;

	start_ = qi::lexeme[(qi::long_long >> -suffix_symbols_)[qi::_val = boost::phoenix::bind(&numeric_helper::make_int_node, qi::_1, qi::_2)]];

	suffix_symbols_.add
		("i8", integral_suffix_type::i8)
		("ui8", integral_suffix_type::ui8)
		("i16", integral_suffix_type::i16)
		("ui16", integral_suffix_type::ui16)
		("i32", integral_suffix_type::i32)
		("ui32", integral_suffix_type::ui32)
		("i64", integral_suffix_type::i64)
		("ui64", integral_suffix_type::ui64)
		("i128", integral_suffix_type::i128)
		("ui128", integral_suffix_type::ui128);
}

oosl::lexer::real_literal_grammar::real_literal_grammar()
	: grammar("OOSL_REAL_LITERAL"){
	using namespace boost::spirit;

	start_ = qi::lexeme[(real_parser_ >> -suffix_symbols_)[qi::_val = boost::phoenix::bind(&numeric_helper::make_real_node, qi::_1, qi::_2)]];

	suffix_symbols_.add
		("f32", real_suffix_type::f32)
		("f64", real_suffix_type::f64)
		("f128", real_suffix_type::f128);
}

oosl::lexer::numeric_literal_grammar::numeric_literal_grammar()
	: grammar("OOSL_NUMERIC_LITERAL"){
	using namespace boost::spirit;

	start_ %= (rad_ | real_ | integer_);
}

oosl::lexer::raw_literal_grammar::raw_literal_grammar()
	: grammar("OOSL_RAW_LITERAL"){
	using namespace boost::spirit;

	expression_ = std::make_shared<expression_grammar>();

	start_ = ('`' > (*expression_) > '`')[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::raw_literal_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> node_type;
	return std::make_shared<node_type>(node_id_type::literal_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			//*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(owner.value());
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("`");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write("`");
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::quote_literal_grammar::quote_literal_grammar()
	: grammar("OOSL_QUOTE_LITERAL"){
	using namespace boost::spirit;

	start_ %= (character_ | string_ | raw_);
}

oosl::lexer::literal_grammar::literal_grammar()
	: grammar("OOSL_LITERAL"){
	using namespace boost::spirit;

	start_ %= (numeric_ | quote_);
}
