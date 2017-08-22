#pragma once

#ifndef OOSL_ANY_TYPE_H
#define OOSL_ANY_TYPE_H

#include "dynamic_type.h"

namespace oosl{
	namespace type{
		class any : public dynamic{
		public:
			any();

			virtual ~any();

			virtual int score(object &type) override;

			virtual bool is_any() override;
		};
	}
}

#endif /* !OOSL_ANY_TYPE_H */
