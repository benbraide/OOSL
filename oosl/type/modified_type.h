#pragma once

#ifndef OOSL_MODIFIED_TYPE_H
#define OOSL_MODIFIED_TYPE_H

#include "type_object.h"

namespace oosl{
	namespace type{
		class modified : public object{
		public:
			modified(ptr_type underlying_type, attribute attributes);

			virtual ~modified();

			virtual driver_type *driver() override;

			virtual object *underlying_type() override;

			virtual object *non_modified() override;

			virtual object *non_variadic() override;

			virtual object &function_return_type() override;

			virtual ptr_list_type &function_parameters() override;

			virtual std::string name() override;

			virtual std::string print() override;

			virtual size_type size() override;

			virtual int score(object &type) override;

			virtual int score(storage_entry_type &entry) override;

			virtual int score(const storage_entry_list_type &entries) override;

			virtual attribute attributes() override;

			virtual id_type id() override;

			virtual bool is(id_type id) override;

			virtual bool is(attribute attributes) override;

			virtual bool is_any(attribute attributes) override;

			virtual bool is_variant() override;

			virtual bool is_void() override;

			virtual bool is_any() override;

			virtual bool is_auto() override;

			virtual bool is_variadic() override;

			virtual bool is_enum() override;

			virtual bool is_union() override;

			virtual bool is_struct() override;

			virtual bool is_class() override;

			virtual bool is_primitive() override;

			virtual bool is_dynamic() override;

			virtual bool is_numeric() override;

			virtual bool is_integral() override;

			virtual bool is_unsigned_integral() override;

			virtual bool is_floating_point() override;

			virtual bool is_pointer() override;

			virtual bool is_strong_pointer() override;

			virtual bool is_string() override;

			virtual bool is_wstring() override;

			virtual bool is_array() override;

			virtual bool is_strong_array() override;

			virtual bool is_static_array() override;

			virtual bool is_function() override;

			virtual bool is_strong_function() override;

			virtual bool is_nullptr() override;

			virtual bool is_nan() override;

			virtual bool is_modified() override;

			virtual attribute &attributes_ref();

		protected:
			ptr_type underlying_type_;
			attribute attributes_;
		};
	}
}

#endif /* !OOSL_MODIFIED_TYPE_H */
