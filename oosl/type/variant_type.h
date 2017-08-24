#pragma once

#ifndef OOSL_VARIANT_TYPE_H
#define OOSL_VARIANT_TYPE_H

#include "type_object.h"

namespace oosl{
	namespace type{
		class variant : public object{
		public:
			explicit variant(ptr_list_type &&underlying_types);

			virtual ~variant();

			virtual driver_type *driver();

			virtual object *underlying_type();

			virtual std::string name();

			virtual std::string print();

			virtual size_type size();

			virtual int score(object &type);

			virtual object *match(object &type, match_type criteria = match_type::same);

			virtual bool is_variant();

		protected:
			ptr_list_type underlying_types_;
		};
	}
}

#endif /* !OOSL_VARIANT_TYPE_H */
