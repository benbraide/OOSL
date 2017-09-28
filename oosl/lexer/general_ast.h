#pragma once

#ifndef OOSL_GENERAL_AST_H
#define OOSL_GENERAL_AST_H

#include "../node/inplace_node.h"
#include "../driver/driver_object.h"
#include "../common/raii.h"

#include "ast.h"
#include "literal_ast.h"

namespace oosl{
	namespace lexer{
		struct lookup_helper{
			static oosl::storage::object::entry_type *lookup(const std::string &key){
				auto &runtime_info = oosl::common::controller::active->runtime_info();
				auto value = runtime_info.storage->find(key, runtime_info.find_type);
				if (value != nullptr){
					auto object_value = value->object();
					if (object_value == nullptr){//Not an object
						auto type_value = value->type();
						if (type_value == nullptr)//Return storage object
							return oosl::common::controller::active->temporary_storage().add_scalar(*value->storage());

						return oosl::common::controller::active->temporary_storage().add_scalar(*type_value);//Return type object
					}
					
					return object_value;//Is object
				}
				
				throw oosl::common::error_codes::not_found;//Not found
			}
		};

		struct OOSL_AST_NAME(identifier){
			std::string value;
		};

		OOSL_AST_TO_NODE(identifier){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<std::string> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(identifier) &ast){
				return std::make_shared<inplace_type>(node_id_type::identifier, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						*reinterpret_cast<entry_type **>(out) = lookup_helper::lookup(owner.value());
						return true;
					case inplace_target_type::print:
					case inplace_target_type::key:
						*reinterpret_cast<std::string *>(out) = owner.value();
						return true;
					default:
						break;
					}

					return false;
				}, ast.value);
			}
		};

		struct OOSL_AST_NAME(placeholder){
			typedef boost::variant<
				OOSL_AST_NAME(string),
				OOSL_AST_NAME(identifier)
			> value_type;
			value_type value;
		};

		OOSL_AST_VISITOR_2(placeholder, string, identifier);

		OOSL_AST_TO_NODE(placeholder){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<node_ptr_type> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(placeholder) &ast){
				return std::make_shared<inplace_type>(node_id_type::placeholder, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						*reinterpret_cast<entry_type **>(out) = lookup_helper::lookup(owner.key());
						return true;
					case inplace_target_type::print:
						*reinterpret_cast<std::string *>(out) = ("__placeholder(" + owner.value()->print() + ")");
						return true;
					case inplace_target_type::key:
					{//Evaluate value and return string value
						auto value = owner.value()->evaluate();
						*reinterpret_cast<std::string *>(out) = value->type->driver()->value<std::string>(*value);
						return true;
					}
					default:
						break;
					}

					return false;
				}, OOSL_AST_APPLY_VISITOR(placeholder, ast.value));
			}
		};

		struct OOSL_AST_NAME(identifier_compatible){
			typedef boost::variant<
				OOSL_AST_NAME(placeholder),
				OOSL_AST_NAME(identifier)
			> value_type;
			value_type value;
		};

		OOSL_AST_VISITOR_2(identifier_compatible, placeholder, identifier);

		OOSL_AST_TO_NODE(identifier_compatible){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(identifier_compatible) &ast){
				return OOSL_AST_APPLY_VISITOR(identifier_compatible, ast.value);
			}
		};

		struct OOSL_AST_NAME(global_qualified){
			OOSL_AST_NAME(identifier_compatible) value;
		};

		OOSL_AST_TO_NODE(global_qualified){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<node_ptr_type> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(global_qualified) &ast){
				auto identifier_node = OOSL_AST_TO_NODE_GET(ast.value);
				return std::make_shared<inplace_type>(node_id_type::qualified, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
					{//Evaluate using global storage
						auto &runtime_info = oosl::common::controller::active->runtime_info();

						oosl::common::raii_state<oosl::storage::object *> storage_state(runtime_info.storage, &oosl::common::controller::active->global_storage());
						oosl::common::raii_state<oosl::storage::object::find_type> find_state(runtime_info.find_type, oosl::storage::object::find_type::nil);//Disable recursive search

						*reinterpret_cast<entry_type **>(out) = owner.value()->evaluate();
						return true;
					}
					case inplace_target_type::print:
						*reinterpret_cast<std::string *>(out) = ("::" + owner.value()->print());
						return true;
					default:
						break;
					}

					return false;
				}, identifier_node);
			}
		};

		struct qualified_ast_to_node_helper{
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef std::pair<node_ptr_type, node_ptr_type> node_ptr_pair_type;

			typedef oosl::node::inplace<node_ptr_pair_type> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static node_ptr_type make_node(node_ptr_type left, node_ptr_type right, const node_index_type &index){
				return std::make_shared<inplace_type>(node_id_type::qualified, index, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
					{//Evaluate using specified storage
						auto &value = owner.value();
						auto storage = value.first->storage();
						if (storage == nullptr)//Not found
							throw oosl::common::error_codes::storage_not_found;

						auto &runtime_info = oosl::common::controller::active->runtime_info();
						oosl::common::raii_state<oosl::storage::object *> storage_state(storage, &oosl::common::controller::active->global_storage());
						oosl::common::raii_state<oosl::storage::object::find_type> find_state(runtime_info.find_type, oosl::storage::object::find_type::nil);//Disable recursive search

						*reinterpret_cast<entry_type **>(out) = value.second->evaluate();
						return true;
					}
					case inplace_target_type::print:
					{
						auto &value = owner.value();
						*reinterpret_cast<std::string *>(out) = (value.first->print() + "::" + value.second->print());
						return true;
					}
					default:
						break;
					}

					return false;
				}, std::make_pair(left, right));
			}
		};

		struct OOSL_AST_NAME(recursive_qualified){
			typedef boost::variant<
				boost::recursive_wrapper<OOSL_AST_NAME(recursive_qualified)>,
				OOSL_AST_NAME(identifier_compatible)
			> right_type;
			OOSL_AST_NAME(identifier_compatible) left;
			right_type right;
		};

		OOSL_AST_VISITOR_INTERFACE_2(recursive_qualified, recursive_qualified, identifier_compatible);

		OOSL_AST_TO_NODE(recursive_qualified){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(recursive_qualified) &ast){
				auto left_node = OOSL_AST_TO_NODE_GET(ast.left);
				auto right_node = OOSL_AST_APPLY_VISITOR(recursive_qualified, ast.right);
				return qualified_ast_to_node_helper::make_node(left_node, right_node, oosl::node::index{});
			}
		};

		struct OOSL_AST_NAME(relative_qualified){
			typedef boost::variant<
				OOSL_AST_NAME(global_qualified),
				OOSL_AST_NAME(identifier_compatible)
			> left_type;

			typedef boost::variant<
				OOSL_AST_NAME(recursive_qualified),
				OOSL_AST_NAME(identifier_compatible)
			> right_type;

			left_type left;
			right_type right;
		};

		OOSL_AST_VISITOR_2(relative_qualified_left, global_qualified, identifier_compatible);
		OOSL_AST_VISITOR_2(relative_qualified_right, recursive_qualified, identifier_compatible);

		OOSL_AST_TO_NODE(relative_qualified){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(relative_qualified) &ast){
				auto left_node = OOSL_AST_APPLY_VISITOR(relative_qualified_left, ast.left);
				auto right_node = OOSL_AST_APPLY_VISITOR(relative_qualified_right, ast.right);
				return qualified_ast_to_node_helper::make_node(left_node, right_node, oosl::node::index{});
			}
		};

		struct OOSL_AST_NAME(qualified){
			typedef boost::variant<
				OOSL_AST_NAME(relative_qualified),
				OOSL_AST_NAME(global_qualified)
			> value_type;
			value_type value;
		};

		OOSL_AST_VISITOR_2(qualified, relative_qualified, global_qualified);

		OOSL_AST_TO_NODE(qualified){
			static oosl::node::object::ptr_type get(OOSL_AST_NAME(qualified) &ast){
				return OOSL_AST_APPLY_VISITOR(qualified, ast.value);
			}
		};

		struct OOSL_AST_NAME(system_call){
			unsigned int value;
		};

		OOSL_AST_TO_NODE(system_call){
			typedef oosl::node::object::ptr_type node_ptr_type;
			typedef oosl::node::id node_id_type;
			typedef oosl::node::index node_index_type;

			typedef oosl::node::inplace<unsigned int> inplace_type;
			typedef oosl::node::inplace_target_type inplace_target_type;
			typedef oosl::storage::object::entry_type entry_type;

			static oosl::node::object::ptr_type get(OOSL_AST_NAME(system_call) &ast){
				return std::make_shared<inplace_type>(node_id_type::placeholder, node_index_type{}, [](inplace_type &owner, inplace_target_type target, void *out) -> bool{
					switch (target){
					case inplace_target_type::eval:
						//*reinterpret_cast<entry_type **>(out) = lookup_helper::lookup(owner.key());
						return true;
					case inplace_target_type::print:
						*reinterpret_cast<std::string *>(out) = ("__call(" + std::to_string(owner.value()) + ")");
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
	OOSL_AST_QNAME(identifier),
	(std::string, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(placeholder),
	(OOSL_AST_QNAME(placeholder)::value_type, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(identifier_compatible),
	(OOSL_AST_QNAME(identifier_compatible)::value_type, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(global_qualified),
	(OOSL_AST_QNAME(identifier_compatible), value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(recursive_qualified),
	(OOSL_AST_QNAME(identifier_compatible), left)
	(OOSL_AST_QNAME(recursive_qualified)::right_type, right)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(relative_qualified),
	(OOSL_AST_QNAME(relative_qualified)::left_type, left)
	(OOSL_AST_QNAME(relative_qualified)::right_type, right)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(qualified),
	(OOSL_AST_QNAME(qualified)::value_type, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(system_call),
	(unsigned int, value)
)

#endif /* !OOSL_GENERAL_AST_H */
