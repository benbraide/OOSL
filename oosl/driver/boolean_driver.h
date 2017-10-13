#pragma once

#ifndef OOSL_BOOLEAN_DRIVER_H
#define OOSL_BOOLEAN_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class boolean : public object{
		public:
			using object::echo;
			using object::value;

			virtual ~boolean();

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info) override;

			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand) override;
		};
	}
}

#endif /* !OOSL_BOOLEAN_DRIVER_H */
