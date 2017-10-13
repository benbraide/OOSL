#pragma once

#ifndef OOSL_BYTE_DRIVER_H
#define OOSL_BYTE_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class byte : public object{
		public:
			using object::echo;
			using object::value;

			virtual ~byte();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info) override;

			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand) override;

			virtual entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, uint8_type operand);
		};
	}
}

#endif /* !OOSL_BYTE_DRIVER_H */
