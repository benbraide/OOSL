#pragma once

#ifndef OOSL_NAMED_STORAGE_H
#define OOSL_NAMED_STORAGE_H

#include "storage_object.h"

namespace oosl{
	namespace storage{
		class named : public object{
		public:
			explicit named(const std::string &name, object *parent = nullptr);

			virtual ~named();

			virtual object *match(const std::string &name) override;

			virtual value_type *find(const std::string &key, find_type type = find_type::nil) override;

			virtual const std::string &name() const;

			virtual std::string print() const;

		protected:
			std::string name_;
			value_type this_value_;
		};
	}
}

#endif /* !OOSL_NAMED_STORAGE_H */
