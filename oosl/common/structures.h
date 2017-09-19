#pragma once

#ifndef OOSL_STRUCTURES_H
#define OOSL_STRUCTURES_H

#include "../storage/temporary_storage.h"

namespace oosl{
	namespace common{
		struct interpreter_info{};

		struct runtime_block_info{};

		struct runtime_info{
			storage::temporary *temporary_storage;
		};
	}
}

#endif /* !OOSL_STRUCTURES_H */
