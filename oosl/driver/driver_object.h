#pragma once

#ifndef OOSL_DRIVER_OBJECT_H
#define OOSL_DRIVER_OBJECT_H

#include "../storage/storage_entry.h"

namespace oosl{
	namespace driver{
		class object{
		public:
			typedef oosl::type::object type_object_type;

			typedef common::error_codes error_type;
			typedef common::controller controller_type;

			typedef storage::entry entry_type;
			typedef entry_type::attribute_type attribute_type;

			typedef std::size_t size_type;
			typedef unsigned __int64 uint64_type;

			enum class cast_option_type : unsigned int{
				nil				= (0 << 0x0000),
				reinterpret		= (1 << 0x0000),
				is_explicit		= (1 << 0x0001),
				ref				= (1 << 0x0002),
				val				= (1 << 0x0003),
				make_copy		= (1 << 0x0004),
			};

			virtual ~object();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil);

			virtual type_object_type *type(entry_type &entry);

			virtual size_type size(entry_type &entry);

			virtual uint64_type address(entry_type &entry);

			virtual entry_type *linked(entry_type &entry);

			virtual attribute_type attributes(entry_type &entry);

			virtual bool is_lval(entry_type &entry);

			virtual bool is_ref(entry_type &entry);

			virtual bool is_uninitialized(entry_type &entry);

			virtual bool is_tls(entry_type &entry);

			virtual bool is_nan(entry_type &entry);

			virtual bool is_void(entry_type &entry);

			virtual uint64_type value(entry_type &entry);

			template <typename target_type>
			target_type typed_value(entry_type &entry){
				return static_cast<target_type>(value(entry));
			}
		};

		OOSL_MAKE_OPERATORS(object::cast_option_type);
	}
}

#endif /* !OOSL_DRIVER_OBJECT_H */
