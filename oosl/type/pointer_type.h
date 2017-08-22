#pragma once

#ifndef OOSL_POINTER_TYPE_H
#define OOSL_POINTER_TYPE_H

#include "primitive_type.h"

namespace oosl{
	namespace type{
		class pointer : public primitive{
		public:
			explicit pointer(ptr_type underlying_type);

			virtual ~pointer();

			virtual driver_type *driver() override;

			virtual object *underlying_type() override;

			virtual std::string name() override;

			virtual std::string print() override;

			virtual int score(object &type) override;

			virtual bool is_dynamic() override;

			virtual bool is_pointer() override;

			virtual bool is_strong_pointer() override;

		protected:
			virtual std::string print_(const std::string &value);

			ptr_type underlying_type_;
		};
	}
}

#endif /* !OOSL_POINTER_TYPE_H */
