#pragma once

#ifndef OOSL_DRIVER_OBJECT_H
#define OOSL_DRIVER_OBJECT_H

#include "../type/type_mapper.h"
#include "../type/custom_types.h"
#include "../type/pointer_type.h"
#include "../common/operator_info.h"
#include "../storage/temporary_storage.h"

namespace oosl{
	namespace driver{
		class object{
		public:
			typedef oosl::type::object type_object_type;
			typedef oosl::type::id type_id_type;
			typedef oosl::type::bool_type bool_type;

			typedef type_object_type::ptr_type type_ptr_type;
			typedef oosl::memory::value_dependency<type_ptr_type> type_value_dependency_type;
			typedef oosl::memory::block::attribute_type memory_attribute_type;

			typedef common::error_codes error_type;
			typedef common::controller controller_type;

			typedef common::operator_id operator_id_type;
			typedef common::unary_operator_info unary_operator_info_type;
			typedef common::binary_operator_info binary_operator_info_type;

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

			virtual entry_type *evaluate(entry_type &entry, unary_operator_info_type &operator_info);

			virtual entry_type *evaluate(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand);

			virtual entry_type *assign(entry_type &entry, entry_type &value);

			virtual void initialize(entry_type &entry);

			virtual bool is_lval(entry_type &entry);

			virtual bool is_ref(entry_type &entry);

			virtual bool is_uninitialized(entry_type &entry);

			virtual bool is_tls(entry_type &entry);

			virtual bool is_nan(entry_type &entry);

			virtual bool is_void(entry_type &entry);

			virtual void value(entry_type &entry, type_id_type to, char *destination);

			template <typename target_type>
			void value(entry_type &entry, target_type &val){
				value(entry, type::mapper<target_type>::id, reinterpret_cast<char *>(&val));
			}

			template <typename target_type>
			target_type value(entry_type &entry){
				target_type val;
				value(entry, val);
				return val;
			}

		protected:
			virtual entry_type *evaluate_(entry_type &entry, unary_operator_info_type &operator_info);

			virtual entry_type *evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand);

			virtual entry_type *assign_(entry_type &entry, entry_type &value);
		};

		OOSL_MAKE_OPERATORS(object::cast_option_type);
	}
}

#endif /* !OOSL_DRIVER_OBJECT_H */
