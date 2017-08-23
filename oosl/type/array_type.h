#pragma once

#ifndef OOSL_ARRAY_TYPE_H
#define OOSL_ARRAY_TYPE_H

#include "dynamic_type.h"

namespace oosl{
	namespace type{
		class array_type : public dynamic{
		public:
			explicit array_type(ptr_type underlying_type);

			virtual ~array_type();

			virtual object *underlying_type() override;

			virtual std::string name() override;

			virtual std::string print() override;

			virtual int score(object &type) override;

			virtual bool is_array() override;

			virtual bool is_strong_array() override;

		protected:
			ptr_type underlying_type_;
		};
	}
}

#endif /* !OOSL_ARRAY_TYPE_H */
