#pragma once

#ifndef OOSL_STRUCTURES_H
#define OOSL_STRUCTURES_H

#include "../node/node_object.h"
#include "../storage/storage_object.h"
#include "../storage/temporary_storage.h"

namespace oosl{
	namespace common{
		struct interpreter_info{};

		struct runtime_block_info{};

		struct runtime_info{
			oosl::storage::object *storage;
			oosl::storage::temporary *temporary_storage;
			oosl::node::object::ptr_type column;
			std::size_t print_indentation;
		};
	}
}

#endif /* !OOSL_STRUCTURES_H */
