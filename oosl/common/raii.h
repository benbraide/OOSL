#pragma once

#ifndef OOSL_RAII_H
#define OOSL_RAII_H

#include <functional>

namespace oosl{
	namespace common{
		class raii{
		public:
			typedef std::function<void()> callback_type;

			raii(callback_type init, callback_type uninit);

			~raii();

		private:
			callback_type uninit_;
		};
	}
}

#endif /* !OOSL_RAII_H */
