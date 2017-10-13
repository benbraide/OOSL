#pragma once

#ifndef OOSL_DYNAMIC_DRIVER_H
#define OOSL_DYNAMIC_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class dynamic_driver : public object{
		public:
			typedef oosl::memory::value_dependency<entry_type> dependency_type;

			virtual ~dynamic_driver();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual type_object_type *type(entry_type &entry) override;

			virtual size_type size(entry_type &entry) override;

			virtual entry_type *linked(entry_type &entry) override;

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info) override;

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info, entry_type &operand) override;

			virtual entry_type *assign(entry_type &entry, entry_type &value) override;

			virtual void initialize(entry_type &entry) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *assign_(entry_type &entry, entry_type &value) override;
		};
	}
}

#endif /* !OOSL_DYNAMIC_DRIVER_H */
