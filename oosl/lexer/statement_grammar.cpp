#include "statement_grammar.h"

oosl::lexer::parameter_declaration_grammar::parameter_declaration_grammar()
	: grammar("OOSL_PARAMETER_DECLARATION"){
	using namespace boost::spirit;

	start_ = (parameter_type_ >> -identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return nullptr;
}

oosl::lexer::variadic_declaration_grammar::variadic_declaration_grammar()
	: grammar("OOSL_VARIADIC_DECLARATION"){
	using namespace boost::spirit;

	start_ = (variadic_type_ >> -identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::variadic_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return nullptr;
}

oosl::lexer::parameter_init_declaration_grammar::parameter_init_declaration_grammar()
	: grammar("OOSL_PARAMETER_INIT_DECLARATION"){
	using namespace boost::spirit;

	start_ = (parameter_declaration_ >> '=' >> non_list_expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_init_declaration_grammar::create(node_ptr_type type, node_ptr_type init){
	return nullptr;
}

oosl::lexer::declaration_grammar::declaration_grammar()
	: grammar("OOSL_DECLARATION"){
	using namespace boost::spirit;

	base_ = (-storage_specifier_ >> (type_ | identifier_compatible_) >> identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3)];

	start_ = (base_ >> -('=' >> non_list_expression_))[qi::_val = boost::phoenix::bind(&create_init, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::declaration_grammar::create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id){
	return nullptr;
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::declaration_grammar::create_init(node_ptr_type value, boost::optional<node_ptr_type> init){
	if (!init.is_initialized())
		return value;
	return nullptr;
}

oosl::lexer::ref_declaration_grammar::ref_declaration_grammar()
	: grammar("OOSL_REF_DECLARATION"){
	using namespace boost::spirit;

	start_ = (-storage_specifier_ >> ref_type_ >> identifier_or_placeholder_ >> '=' >> non_list_expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3, qi::_4)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::ref_declaration_grammar::create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init){
	return nullptr;
}

oosl::lexer::auto_declaration_grammar::auto_declaration_grammar()
	: grammar("OOSL_AUTO_DECLARATION"){
	using namespace boost::spirit;

	start_ = (-storage_specifier_ >> (ref_auto_type_ | auto_type_) >> identifier_or_placeholder_ >> '=' >> non_list_expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3, qi::_4)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::auto_declaration_grammar::create(boost::optional<type_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init){
	return nullptr;
}
