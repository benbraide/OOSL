#include "general_statement_grammar.h"
#include "../common/structures.h"

oosl::lexer::return_statement_grammar::return_statement_grammar()
	: grammar("OOSL_RETURN_STATEMENT"){
	using namespace boost::spirit;

	start_ = (keyword("return") >> -expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::return_statement_grammar::create(boost::optional<node_ptr_type> value){
	typedef oosl::node::inplace<node_ptr_type> node_type;
	return std::make_shared<node_type>(node_id_type::return_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
		{
			auto &controller = *oosl::common::controller::active;
			auto &runtime_info = controller.runtime_info();

			if (owner.value() == nullptr){//Return value
				runtime_info.exception_value = owner.value()->evaluate();
				++controller.memory().find_block(runtime_info.exception_value->address)->ref_count;
			}
			else//Return void
				runtime_info.exception_value = nullptr;

			throw oosl::common::error_codes::return_;
			return true;
		}
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("return");
			if (owner.value() != nullptr){//Print value
				reinterpret_cast<output_writer_type *>(out)->write(" ");
				owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			}
			return true;
		default:
			break;
		}

		return false;
	}, value.value_or(nullptr));
}

oosl::lexer::throw_statement_grammar::throw_statement_grammar()
	: grammar("OOSL_THROW_STATEMENT"){
	using namespace boost::spirit;

	start_ = (keyword("throw") >> expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::throw_statement_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> node_type;
	return std::make_shared<node_type>(node_id_type::throw_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
		{
			auto &controller = *oosl::common::controller::active;
			auto &runtime_info = controller.runtime_info();

			runtime_info.exception_value = owner.value()->evaluate();
			++controller.memory().find_block(runtime_info.exception_value->address)->ref_count;

			throw oosl::common::error_codes::throw_;
			return true;
		}
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("throw ");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::rethrow_statement_grammar::rethrow_statement_grammar()
	: grammar("OOSL_RETHROW_STATEMENT"){
	using namespace boost::spirit;

	start_ = keyword("throw")[qi::_val = create()];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::rethrow_statement_grammar::create(){
	typedef oosl::node::inplace<void> node_type;
	return std::make_shared<node_type>(node_id_type::throw_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			throw oosl::common::error_codes::throw_;
			return true;
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("throw");
			return true;
		default:
			break;
		}

		return false;
	});
}

oosl::lexer::case_statement_grammar::case_statement_grammar()
	: grammar("OOSL_CASE_STATEMENT"){
	using namespace boost::spirit;

	start_ = (keyword("case") >> expression_ >> ":")[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::case_statement_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> node_type;
	return std::make_shared<node_type>(node_id_type::case_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("case ");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			reinterpret_cast<output_writer_type *>(out)->write(":");
			return true;
		default:
			break;
		}

		return false;
	}, value);
}

oosl::lexer::default_statement_grammar::default_statement_grammar()
	: grammar("OOSL_DEFAULT_STATEMENT"){
	using namespace boost::spirit;

	start_ = (keyword("default") >> qi::lit(":"))[qi::_val = create()];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::default_statement_grammar::create(){
	typedef oosl::node::inplace<void> node_type;
	return std::make_shared<node_type>(node_id_type::case_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("default:");
			return true;
		default:
			break;
		}

		return false;
	});
}

oosl::lexer::echo_statement_grammar::echo_statement_grammar()
	: grammar("OOSL_ECHO_STATEMENT"){
	using namespace boost::spirit;

	start_ = (keyword("echo") >> expression_)[qi::_val = boost::phoenix::bind(&create, qi::_1)];
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::echo_statement_grammar::create(node_ptr_type value){
	typedef oosl::node::inplace<node_ptr_type> node_type;
	return std::make_shared<node_type>(node_id_type::case_, [](node_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
		{
			auto value = owner.value()->evaluate();
			value->type->driver()->echo(*value);
			return true;
		}
		case inplace_target_type::print:
			reinterpret_cast<output_writer_type *>(out)->write("echo ");
			owner.value()->echo(*reinterpret_cast<output_writer_type *>(out));
			return true;
		default:
			break;
		}

		return false;
	}, value);
}
