#pragma once

#ifndef OOSL_CONSTANT_AST_H
#define OOSL_CONSTANT_AST_H

#include "../node/inplace_node.h"

#include "ast.h"

namespace oosl{
	namespace lexer{
		enum class constant_type{
			nil,
			false_,
			true_,
			indeterminate,
			nullptr_,
			nan_,
			undefined,
		};

		struct OOSL_AST_NAME(constant){
			constant_type value;
		};

		OOSL_AST_TO_NODE(constant){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<constant_type> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			typedef oosl::type::bool_type bool_type;
			typedef oosl::type::other_type other_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(constant) &ast){
				return std::make_shared<inplace_type>(node_id_type::constant, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
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
							*reinterpret_cast<std::string *>(out) = "false";
							break;
						case constant_type::true_:
							*reinterpret_cast<std::string *>(out) = "true";
							break;
						case constant_type::indeterminate:
							*reinterpret_cast<std::string *>(out) = "indeterminate";
							break;
						case constant_type::nullptr_:
							*reinterpret_cast<std::string *>(out) = "nullptr";
							break;
						case constant_type::nan_:
							*reinterpret_cast<std::string *>(out) = "NaN";
							break;
						case constant_type::undefined:
							*reinterpret_cast<std::string *>(out) = "undefined";
						default:
							throw oosl::common::error_codes::bad_constant;
							break;
						}

						return true;
					default:
						break;
					}

					return false;
				}, ast.value);
			}
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(constant),
	(oosl::lexer::constant_type, value)
)

#endif /* !OOSL_CONSTANT_AST_H */
