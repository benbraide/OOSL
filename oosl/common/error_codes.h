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
		};
	}
}

#endif /* !OOSL_ERROR_CODES_H */
