#pragma once

#ifndef OOSL_DYNAMIC_TYPE_H
#define OOSL_DYNAMIC_TYPE_H

#include "primitive_type.h"

namespace oosl{
	namespace type{
		class dynamic : public primitive{
		public:
			explicit dynamic(id_type id);

			virtual ~dynamic();

			virtual driver_type *driver() override;

			virtual int score(object &type) override;

			virtual bool is_dynamic() override;
		};
	}
}

#endif /* !OOSL_DYNAMIC_TYPE_H */
