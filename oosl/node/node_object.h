#pragma once

#ifndef OOSL_NODE_OBJECT_H
#define OOSL_NODE_OBJECT_H

#include <memory>
#include <functional>

#include "../type/type_object.h"
#include "../storage/storage_object.h"

#include "../common/raii.h"
#include "../common/controller.h"
#include "../common/output_writer.h"

#include "node_id.h"
#include "node_index.h"

namespace oosl{
	namespace node{
		class object : public std::enable_shared_from_this<object>{
		public:
			typedef id id_type;
			typedef index index_type;

			typedef oosl::type::object type_object_type;
			typedef type_object_type::size_type size_type;

			typedef oosl::storage::object storage_type;
			typedef storage_type::entry_type entry_type;

			typedef oosl::common::error_codes error_type;
			typedef common::output_writer output_writer_type;
			typedef output_writer_type::write_option_type write_option_type;
			typedef oosl::common::controller::output_writer_key_type output_writer_key_type;

			typedef std::shared_ptr<object> ptr_type;
			typedef std::function<bool(ptr_type)> traverser_type;

			virtual ~object();

			virtual ptr_type reflect();

			virtual id_type id();

			virtual index_type &index();

			virtual bool is(id_type id);

			virtual void echo();

			virtual void echo(output_writer_type &writer);

			virtual std::string key();

			virtual entry_type *evaluate();

			virtual storage_type *storage();

			virtual type_object_type *type();

			virtual size_type count();

			virtual void traverse(traverser_type traverser);

		protected:
			virtual entry_type *evaluate_();
		};
	}
}

#endif /* !OOSL_NODE_OBJECT_H */
