#pragma once

#ifndef OOSL_GENERAL_AST_H
#define OOSL_GENERAL_AST_H

#include "../node/inplace_node.h"

#include "ast.h"

namespace oosl{
	namespace lexer{
		struct OOSL_AST_NAME(identifier){
			std::string value;
		};

		struct OOSL_AST_VISITOR_NAME(identifier){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<std::string> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			node_ptr_type operator()(const OOSL_AST_NAME(identifier) &value){
				return std::make_shared<inplace_type>(node_id_type::identifier, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
					{//Find object
						auto &runtime_info = oosl::common::controller::active->runtime_info();
						auto value = runtime_info.storage->find(owner.value(), runtime_info.find_type);
						if (value != nullptr){
							auto object_value = value->object();
							if (object_value == nullptr){//Not an object
								auto type_value = value->type();
								if (type_value == nullptr)//Return storage object
									*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*value->storage());
								else//Return type object
									*reinterpret_cast<entry_type **>(out) = oosl::common::controller::active->temporary_storage().add_scalar(*type_value);
							}
							else//Is object
								*reinterpret_cast<entry_type **>(out) = object_value;
						}
						else//Not found
							throw oosl::common::error_codes::not_found;

						return true;
					}
					case inplace_target_type::print:
					case inplace_target_type::key:
						*reinterpret_cast<std::string *>(out) = owner.value();
						return true;
					default:
						break;
					}

					return false;
				}, value.value);
			}
		};

		struct OOSL_AST_NAME(global_qualified){
			OOSL_AST_NAME(identifier) value;
		};

		struct OOSL_AST_NAME(recursive_qualified){
			typedef boost::variant<
				boost::recursive_wrapper<OOSL_AST_NAME(recursive_qualified)>,
				OOSL_AST_NAME(identifier)
			> right_type;
			OOSL_AST_NAME(identifier) left;
			boost::optional<right_type> right;
		};

		struct OOSL_AST_NAME(relative_qualified){
			typedef boost::variant<
				OOSL_AST_NAME(global_qualified),
				OOSL_AST_NAME(identifier)
			> left_type;
			left_type left;
			OOSL_AST_NAME(recursive_qualified) right;
		};

		struct OOSL_AST_NAME(qualified){
			typedef boost::variant<
				OOSL_AST_NAME(relative_qualified),
				OOSL_AST_NAME(global_qualified)
			> value_type;
			value_type value;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(identifier),
	(std::string, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(global_qualified),
	(OOSL_AST_QNAME(identifier), value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(recursive_qualified),
	(OOSL_AST_QNAME(identifier), left)
	(boost::optional<OOSL_AST_QNAME(recursive_qualified)::right_type>, right)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(relative_qualified),
	(OOSL_AST_QNAME(relative_qualified)::left_type, left)
	(OOSL_AST_QNAME(recursive_qualified), right)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(qualified),
	(OOSL_AST_QNAME(qualified)::value_type, value)
)

#endif /* !OOSL_GENERAL_AST_H */
