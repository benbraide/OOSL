#include "statement_grammar.h"

oosl::lexer::variadic_declaration_grammar::variadic_declaration_grammar()
	: grammar("OOSL_PARAMETER_DECLARATION"){
	using namespace boost::spirit;

	start_ = (variadic_type_ >> -identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::variadic_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return nullptr;
}

oosl::lexer::parameter_declaration_grammar::parameter_declaration_grammar()
	: grammar("OOSL_PARAMETER_DECLARATION"){
	using namespace boost::spirit;

	base_ = ((full_modified_type_ | type_ | identifier_compatible_) >> -identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];

	start_ = (base_ >> -('=' > non_list_expression_))[qi::_val = boost::phoenix::bind(&create_init, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return nullptr;
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_declaration_grammar::create_init(node_ptr_type value, boost::optional<node_ptr_type> init){
	if (!init.is_initialized())
		return value;
	return nullptr;
}

oosl::lexer::explicit_declaration_grammar::explicit_declaration_grammar()
	: grammar("OOSL_EXPLICIT_DECLARATION"){
	using namespace boost::spirit;

	required_ = ((typename_type_ | specified_type_) > identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
	optional_ = ((type_ | identifier_compatible_) >> identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];

	start_ = ((required_ | optional_) >> -('=' > non_list_expression_))[qi::_val = boost::phoenix::bind(&create_init, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::explicit_declaration_grammar::create(node_ptr_type type, node_ptr_type id){
	return nullptr;
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::explicit_declaration_grammar::create_init(node_ptr_type value, boost::optional<node_ptr_type> init){
	if (!init.is_initialized())
		return value;
	return nullptr;
}
