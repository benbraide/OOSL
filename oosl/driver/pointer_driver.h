#pragma once

#ifndef OOSL_POINTER_DRIVER_H
#define OOSL_POINTER_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class pointer : public object{
		public:
			using object::echo;
			using object::value;

			virtual ~pointer();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info, node_ptr_type &operand) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

			virtual entry_type *target_entry(entry_type &entry, long long offset = 0ll);

			virtual long long pre_multiply(entry_type &entry, long long value);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info) override;

			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand) override;

			virtual entry_type *offset_entry_(entry_type &entry, long long offset, bool assign = false);
		};
	}
}

#endif /* !OOSL_POINTER_DRIVER_H */
