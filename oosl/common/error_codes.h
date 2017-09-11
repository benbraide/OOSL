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
			uninitialized_object,
			void_object,
			incompatible_types,
			rval_ref,
			rval_to_lval,
			rval_assignment,
			integer_required,
			unhandled_operator,
			not_implemented,
		};
	}
}

#endif /* !OOSL_ERROR_CODES_H */
