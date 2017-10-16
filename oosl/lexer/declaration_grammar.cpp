#include "declaration_grammar.h"

oosl::lexer::declaration_grammar::declaration_grammar()
	: grammar("OOSL_DECLARATION"){
	using namespace boost::spirit;

	base_ = (-storage_specifier_ >> (type_ | identifier_compatible_) >> identifier_or_placeholder_ >> -('=' >> non_list_expression_))
		[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3, qi::_4)];

	start_ = base_[qi::_val = qi::_1] >> *(identifier_or_placeholder_ >> -('=' >> non_list_expression_))
		[qi::_val = boost::phoenix::bind(&create_multiple, qi::_val, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::declaration_grammar::create(boost::optional<storage_attribute_type> attributes, node_ptr_type type,
	node_ptr_type id, boost::optional<node_ptr_type> init){
	return std::make_shared<oosl::node::declaration>(
		attributes.value_or(storage_attribute_type::nil),
		type,
		id,
		init.value_or(nullptr)
		);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::declaration_grammar::create_multiple(node_ptr_type decl, node_ptr_type id, boost::optional<node_ptr_type> init){
	auto decl_node = dynamic_cast<oosl::node::declaration *>(decl.get());
	return std::make_shared<oosl::node::multiple_declaration>(
		decl_node->attributes(),
		decl_node->type_node(),
		id,
		init.value_or(nullptr)
		);
}

oosl::lexer::auto_or_ref_declaration_grammar::auto_or_ref_declaration_grammar()
	: grammar("OOSL_AUTO_OR_REF_DECLARATION"){
	using namespace boost::spirit;

	base_ = (-storage_specifier_ >> (ref_auto_type_ | auto_type_ | ref_type_) >> identifier_or_placeholder_ >> '=' >> non_list_expression_)
		[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3, qi::_4)];

	start_ = base_[qi::_val = qi::_1] >> *(identifier_or_placeholder_ >> '=' >> non_list_expression_)
		[qi::_val = boost::phoenix::bind(&create_multiple, qi::_val, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::auto_or_ref_declaration_grammar::create(boost::optional<storage_attribute_type> attributes, node_ptr_type type, node_ptr_type id, node_ptr_type init){
	return std::make_shared<oosl::node::declaration>(
		attributes.value_or(storage_attribute_type::nil),
		type,
		id,
		init
		);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::auto_or_ref_declaration_grammar::create_multiple(node_ptr_type decl, node_ptr_type id, node_ptr_type init){
	auto decl_node = dynamic_cast<oosl::node::declaration *>(decl.get());
	return std::make_shared<oosl::node::multiple_declaration>(
		decl_node->attributes(),
		decl_node->type_node(),
		id,
		init
		);
}

oosl::lexer::parameter_declaration_grammar::parameter_declaration_grammar()
	: grammar("OOSL_PARAMETER_DECLARATION"){
	using namespace boost::spirit;

	start_ = (type_ >> identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return std::make_shared<oosl::node::declaration>(
		storage_attribute_type::nil,
		type,
		id.value_or(nullptr),
		nullptr
		);
}

oosl::lexer::parameter_init_declaration_grammar::parameter_init_declaration_grammar()
	: grammar("OOSL_PARAMETER_INIT_DECLARATION"){
	using namespace boost::spirit;

	start_ = (type_ >> identifier_or_placeholder_ >> '=' >> non_list_expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2, qi::_3)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::parameter_init_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id, node_ptr_type init){
	return std::make_shared<oosl::node::declaration>(
		storage_attribute_type::nil,
		type,
		id.value_or(nullptr),
		init
		);
}

oosl::lexer::variadic_declaration_grammar::variadic_declaration_grammar()
	: grammar("OOSL_VARIADIC_DECLARATION"){
	using namespace boost::spirit;

	start_ = (type_ >> identifier_or_placeholder_)[qi::_val = boost::phoenix::bind(&create, qi::_1, qi::_2)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::variadic_declaration_grammar::create(node_ptr_type type, boost::optional<node_ptr_type> id){
	return std::make_shared<oosl::node::declaration>(
		storage_attribute_type::nil,
		type,
		id.value_or(nullptr),
		nullptr
		);
}
