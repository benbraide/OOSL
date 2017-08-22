#pragma once

#ifndef OOSL_NUMERIC_TYPE_H
#define OOSL_NUMERIC_TYPE_H

#include "primitive_type.h"

namespace oosl{
	namespace type{
		class numeric : public primitive{
		public:
			explicit numeric(id_type id);

			virtual ~numeric();

			virtual int score(object &type) override;

			virtual bool is_numeric() override;
		};
	}
}

#endif /* !OOSL_NUMERIC_TYPE_H */
