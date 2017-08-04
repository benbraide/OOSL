#pragma once

#ifndef OOSL_MEMORY_WATCHER_H
#define OOSL_MEMORY_WATCHER_H

#include "../common/range.h"

#include "memory_block.h"

namespace oosl{
	namespace memory{
		class watcher{
		public:
			typedef block::uint64_type uint64_type;
			typedef common::range<uint64_type> range_type;

			virtual ~watcher() = default;

			virtual void on_change(const range_type &range, const range_type &changed) = 0;
		};
	}
}

#endif /* !OOSL_MEMORY_WATCHER_H */
