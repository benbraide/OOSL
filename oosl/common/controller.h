#pragma once

#ifndef OOSL_CONTROLLER_H
#define OOSL_CONTROLLER_H

#include <memory>

namespace oosl{
	namespace type{
		class object;
	}

	namespace storage{
		struct entry;
	}

	namespace driver{
		class object;
	}

	namespace common{
		class controller{
		public:
			typedef oosl::storage::entry storage_entry_type;
			typedef oosl::driver::object driver_type;

			typedef oosl::type::object type_object_type;
			typedef std::shared_ptr<type_object_type> type_object_ptr_type;

			virtual ~controller();

			virtual driver_type &driver(storage_entry_type &entry) = 0;

			static controller *active;
		};
	}
}

#endif /* !OOSL_CONTROLLER_H */
