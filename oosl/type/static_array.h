#pragma once

#ifndef OOSL_STATIC_ARRAY_H
#define OOSL_STATIC_ARRAY_H

#include "array_type.h"

namespace oosl{
	namespace type{
		class static_array : public array_type{
		public:
			static_array(ptr_type underlying_type, size_type count);

			virtual ~static_array();

			virtual driver_type *driver() override;

			virtual std::string name() override;

			virtual std::string print() override;

			virtual size_type size() override;

			virtual bool is_dynamic() override;

			virtual bool is_static_array() override;

			virtual size_type count();

		protected:
			size_type count_;
		};
	}
}

#endif /* !OOSL_STATIC_ARRAY_H */
