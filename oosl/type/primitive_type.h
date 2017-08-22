#pragma once

#ifndef OOSL_PRIMITIVE_TYPE_H
#define OOSL_PRIMITIVE_TYPE_H

#include "type_object.h"

namespace oosl{
	namespace type{
		class primitive : public object{
		public:
			explicit primitive(id_type id);

			virtual ~primitive();

			virtual driver_type *driver() override;

			virtual std::string name() override;

			virtual std::string print();

			virtual size_type size() override;

			virtual int score(object &type) override;

			virtual id_type id() override;

			virtual bool is_primitive() override;

		protected:
			std::string name_;
			size_type size_;
			id_type id_;
		};
	}
}

#endif /* !OOSL_PRIMITIVE_TYPE_H */
