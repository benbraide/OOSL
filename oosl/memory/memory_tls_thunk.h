#pragma once

#ifndef OOSL_MEMORY_TLS_THUNK_H
#define OOSL_MEMORY_TLS_THUNK_H

namespace oosl{
	namespace memory{
		struct block;
		class manager;

		class tls_thunk{
		public:
			virtual ~tls_thunk() = default;

			virtual block *execute(manager &memory_manager) = 0;
		};
	}
}

#endif /* !OOSL_MEMORY_TLS_THUNK_H */
