#pragma once

#ifndef OOSL_DECLARATION_NODE_H
#define OOSL_DECLARATION_NODE_H

#include "node_object.h"
#include "../type/modified_type.h"

namespace oosl{
	namespace node{
		class declaration : public object{
		public:
			typedef oosl::type::object::attribute storage_attribute_type;

			using object::echo;

			declaration(storage_attribute_type attributes, ptr_type type, ptr_type id, ptr_type init);

			virtual ~declaration();

			virtual id_type id() override;

			virtual void echo(output_writer_type &writer) override;

			virtual type_object_type *type() override;

			virtual ptr_type type_node() override;

			virtual storage_attribute_type attributes();

			virtual ptr_type id_node();

			virtual ptr_type init();

		protected:
			virtual entry_type *evaluate_() override;

			storage_attribute_type attributes_;
			ptr_type type_;
			ptr_type id_;
			ptr_type init_;
			entry_type static_value_;
		};

		class multiple_declaration : public declaration{
		public:
			using declaration::echo;

			template <typename... args_type>
			explicit multiple_declaration(args_type &&... args)
				: declaration(std::forward<args_type>(args)...){}

			virtual ~multiple_declaration();

			virtual void echo(output_writer_type &writer) override;
		};
	}
}

#endif /* !OOSL_DECLARATION_NODE_H */
