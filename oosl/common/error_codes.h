#pragma once

#ifndef OOSL_ERROR_CODES_H
#define OOSL_ERROR_CODES_H

namespace oosl{
	namespace common{
		enum class error_codes{
			nil,
			out_of_memory,
			invalid_address,
			read_violation,
			write_violation,
			invalid_memory_size,
			tls_error,
			undefined_object,
			uninitialized_object,
			void_object,
			incompatible_types,
			rval_ref,
			rval_to_lval,
			rval_assignment,
			integer_required,
			unhandled_operator,
			member_access,
			not_implemented,
			driver_not_found,
			writer_not_found,
			bad_constant,
			bad_char,
			bad_range,
			bad_integer,
			number_too_big,
			invalid_suffix,
			not_found,
			storage_not_found,
			type_not_found,
			out_of_bounds,
		};
	}
}

#endif /* !OOSL_ERROR_CODES_H */
