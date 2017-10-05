#include "constant_grammar.h"

oosl::lexer::constant_grammar::constant_grammar()
	: grammar("OOSL_CONSTANT"){
	using namespace boost::spirit;

	start_ = constant_symbols_[qi::_val = boost::phoenix::bind(&create, qi::_1)];

	constant_symbols_.add
		("false", constant_type::false_)
		("true", constant_type::true_)
		("indeterminate", constant_type::indeterminate)
		("nullptr", constant_type::nullptr_)
		("NaN", constant_type::nan_)
		("undefined", constant_type::undefined);
}

oosl::lexer::grammar::node_ptr_type oosl::lexer::constant_grammar::create(constant_type value){
	typedef oosl::node::inplace<constant_type> inplace_type;
	typedef oosl::type::bool_type bool_type;
	typedef oosl::type::other_type other_type;

	return std::make_shared<inplace_type>(node_id_type::constant, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
		switch (target){
		case inplace_target_type::eval:
			switch (owner.value()){
			case constant_type::false_:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(bool_type::false_);
				break;
			case constant_type::true_:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(bool_type::true_);
				break;
			case constant_type::indeterminate:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(bool_type::indeterminate);
				break;
			case constant_type::nullptr_:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(nullptr);
				break;
			case constant_type::nan_:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(other_type::nan_);
				break;
			case constant_type::undefined:
				*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(other_type::undefined);
			default:
				throw oosl::common::error_codes::bad_constant;
				break;
			}

			return true;
		case inplace_target_type::print:
			switch (owner.value()){
			case constant_type::false_:
				reinterpret_cast<output_writer_type *>(out)->write("false");
				break;
			case constant_type::true_:
				reinterpret_cast<output_writer_type *>(out)->write("true");
				break;
			case constant_type::indeterminate:
				reinterpret_cast<output_writer_type *>(out)->write("indeterminate");
				break;
			case constant_type::nullptr_:
				reinterpret_cast<output_writer_type *>(out)->write("nullptr");
				break;
			case constant_type::nan_:
				reinterpret_cast<output_writer_type *>(out)->write("NaN");
				break;
			case constant_type::undefined:
				reinterpret_cast<output_writer_type *>(out)->write("undefined");
			default:
				throw oosl::common::error_codes::bad_constant;
				break;
			}

			return true;
		default:
			break;
		}

		return false;
	}, value);
}
