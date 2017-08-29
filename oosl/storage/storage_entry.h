#pragma once

#ifndef OOSL_STORAGE_ENTRY_H
#define OOSL_STORAGE_ENTRY_H

#include "../type/type_object.h"

namespace oosl{
	namespace storage{
		class object;

		struct entry{
			typedef unsigned __int64 uint64_type;
			typedef type::object::ptr_type type_ptr_type;

			enum class attribute_type : unsigned int{
				nil					= (0 << 0x0000),
				lval				= (1 << 0x0000),
				ref_				= (1 << 0x0001),
				uninitialized		= (1 << 0x0002),
				tls					= (1 << 0x0003),
				nan_				= (1 << 0x0004),
				static_				= (1 << 0x0005),
				void_				= (1 << 0x0006),
				no_free				= (1 << 0x0007),
			};

			object *scope;
			uint64_type address;
			attribute_type attributes;
			type_ptr_type type;
		};

		OOSL_MAKE_OPERATORS(entry::attribute_type);
	}
}

#endif /* !OOSL_STORAGE_ENTRY_H */
