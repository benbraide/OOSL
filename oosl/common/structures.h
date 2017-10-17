#pragma once

#ifndef OOSL_STRUCTURES_H
#define OOSL_STRUCTURES_H

#include <vector>

#include "../node/node_object.h"
#include "../storage/storage_object.h"
#include "../storage/temporary_storage.h"

namespace oosl{
	namespace common{
		struct interpreter_info{
			std::vector<oosl::node::object::ptr_type> nodes;
		};

		struct runtime_block_info{};

		enum class runtime_state : unsigned int{
			nil						= (0 << 0x0000),
			tls_init				= (1 << 0x0000),
		};

		struct runtime_info{
			runtime_state states;
			oosl::storage::object::find_type find_type;
			oosl::storage::entry *find_target;
			oosl::storage::object *storage;
			oosl::storage::temporary *temporary_storage;
			oosl::node::object::ptr_type column;
			std::size_t print_indentation;
			oosl::storage::value *tls_value;
			oosl::node::object::ptr_type decl_init;
			unsigned __int64 address_offset;
			oosl::storage::entry *exception_value;
		};

		OOSL_MAKE_OPERATORS(runtime_state);
	}
}

#endif /* !OOSL_STRUCTURES_H */
