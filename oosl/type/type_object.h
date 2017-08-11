#pragma once

#ifndef OOSL_TYPE_OBJECT_H
#define OOSL_TYPE_OBJECT_H

#include <string>
#include <memory>

#include "../common/preprocessor.h"
#include "../common/error_codes.h"
#include "../common/controller.h"

#include "type_id.h"

#define OOSL_MIN_TYPE_SCORE 0
#define OOSL_MAX_TYPE_SCORE 20

namespace oosl{
	namespace storage{
		struct entry;
	}

	namespace driver{
		class object;
	}

	namespace type{
		class object : public std::enable_shared_from_this<object>{
		public:
			typedef id id_type;

			typedef common::error_codes error_type;
			typedef common::controller controller_type;

			typedef storage::entry storage_entry_type;
			typedef driver::object driver_type;

			typedef std::size_t size_type;
			typedef unsigned __int64 uint64_type;

			typedef std::shared_ptr<object> ptr_type;

			enum class attribute : unsigned int{
				nil				= (0 << 0x0000),
				ref				= (1 << 0x0000),
				rval			= (1 << 0x0001),
				static_			= (1 << 0x0002),
				tls				= (1 << 0x0003),
				explicit_		= (1 << 0x0004),
			};

			enum class match_type{
				same,
				compatible,
				bully,
			};

			virtual ~object() = default;

			virtual ptr_type reflect();

			virtual driver_type *driver();

			virtual object *underlying_type();

			virtual object *non_modified();

			virtual object *non_variadic();

			virtual std::string &name();

			virtual std::string print();

			virtual size_type size();

			virtual int score(object &type);

			virtual int score(storage_entry_type &entry);

			virtual object *match(object &type, match_type criteria = match_type::same);

			virtual object *match(storage_entry_type &entry, match_type criteria = match_type::same);

			virtual attribute attributes();

			virtual id_type id();

			virtual bool is(id_type id);

			virtual bool is(attribute attributes);

			virtual bool is_any(attribute attributes);

			virtual bool is_same(object &type);

			virtual bool is_compatible(object &type);

			virtual bool is_compatible(storage::entry &entry);

			virtual bool is_class_compatible(object &type);

			virtual bool is_variant();

			virtual bool is_void();

			virtual bool is_any();

			virtual bool is_auto();

			virtual bool is_variadic();

			virtual bool is_enum();

			virtual bool is_union();

			virtual bool is_struct();

			virtual bool is_class();

			virtual bool is_primitive();

			virtual bool is_dynamic();

			virtual bool is_numeric();

			virtual bool is_integral();

			virtual bool is_unsigned_integral();

			virtual bool is_floating_point();

			virtual bool is_pointer();

			virtual bool is_strong_pointer();

			virtual bool is_string();

			virtual bool is_wstring();

			virtual bool is_array();

			virtual bool is_strong_array();

			virtual bool is_static_array();

			virtual bool is_function();

			virtual bool is_strong_function();

			virtual bool is_nullptr();

			virtual bool is_nan();

			virtual bool is_ref();

			virtual bool is_rval_ref();

			virtual bool is_specific();

			virtual bool is_static();

			virtual bool is_thread_local();

			virtual bool is_explicit();

			virtual bool is_modified();
		};

		OOSL_MAKE_OPERATORS(object::attribute);
	}
}

#endif /* !OOSL_TYPE_OBJECT_H */
