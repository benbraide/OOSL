#pragma once

#ifndef OOSL_STORAGE_VALUE_H
#define OOSL_STORAGE_VALUE_H

#include <memory>
#include <variant>

namespace oosl{
	namespace type{
		class object;
	}

	namespace storage{
		struct entry;
		class object;

		class value{
		public:
			typedef oosl::type::object type_object_type;

			typedef oosl::storage::entry entry_type;
			typedef oosl::storage::object object_type;

			typedef std::shared_ptr<type_object_type> type_ptr_type;
			typedef std::shared_ptr<object_type> object_ptr_type;

			typedef std::variant<entry_type, type_ptr_type, object_ptr_type> variant_type;

			template <typename arg_type>
			explicit value(const arg_type &arg)
				: value_(arg){}

			entry_type *object();

			type_object_type *type();

			object_type *storage();

			bool is_object() const;

			bool is_type() const;

			bool is_storage() const;

		protected:
			variant_type value_;
		};
	}
}

#endif /* !OOSL_STORAGE_VALUE_H */
