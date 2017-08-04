#pragma once

#ifndef OOSL_MEMORY_BLOCK_H
#define OOSL_MEMORY_BLOCK_H

#include <cstdlib>

#include "../common/preprocessor.h"

namespace oosl{
	namespace memory{
		struct block{
			typedef unsigned __int64 uint64_type;
			typedef std::size_t size_type;

			enum class attribute_type : unsigned int{
				nil					= (0 << 0x0000),
				allocated			= (1 << 0x0000),
				immutable			= (1 << 0x0001),
				tls					= (1 << 0x0002),
				has_dependency		= (1 << 0x0003),
			};

			size_type ref_count;
			uint64_type address;
			size_type size;
			size_type actual_size;
			attribute_type attributes;
			char *ptr;
		};

		OOSL_MAKE_OPERATORS(block::attribute_type);
	}
}

#endif /* !OOSL_MEMORY_BLOCK_H */
