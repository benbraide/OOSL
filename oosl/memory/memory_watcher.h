#pragma once

#ifndef OOSL_MEMORY_WATCHER_H
#define OOSL_MEMORY_WATCHER_H

#include "../common/range.h"

#include "memory_block.h"

#define OOSL_MM_INT8_SIZE	sizeof(__int8)
#define OOSL_MM_INT16_SIZE	sizeof(__int16)
#define OOSL_MM_INT32_SIZE	sizeof(__int32)
#define OOSL_MM_INT64_SIZE	sizeof(__int64)

#define OOSL_MM_FLT_SIZE	sizeof(float)
#define OOSL_MM_DBL_SIZE	sizeof(double)
#define OOSL_MM_LDBL_SIZE	sizeof(long double)

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
