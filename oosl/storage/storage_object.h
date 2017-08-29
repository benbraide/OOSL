#pragma once

#ifndef OOSL_STORAGE_OBJECT_H
#define OOSL_STORAGE_OBJECT_H

#include <list>
#include <unordered_map>

#include "storage_entry.h"
#include "storage_value.h"

#include "../memory/memory_manager.h"

namespace oosl{
	namespace storage{
		class object{
		public:
			typedef oosl::storage::entry entry_type;
			typedef oosl::storage::value value_type;

			typedef std::shared_ptr<value_type> value_ptr_type;
			typedef std::unordered_map<std::string, value_ptr_type> value_list_type;
			typedef value_list_type::iterator value_list_iterator_type;

			typedef std::pair<const std::string *, value_type *> order_info_type;
			typedef std::list<order_info_type> order_list_type;

			typedef oosl::memory::manager::lock_type lock_type;
			typedef oosl::common::lock_once<lock_type> lock_once_type;

			enum class find_type{
				nil,
				recursive,
			};

			explicit object(object *parent = nullptr);

			virtual ~object();

			virtual object *parent();

			virtual object *match(const std::string &name);

			virtual value_type *find(const std::string &key, find_type type = find_type::nil);

			template <typename target_type, typename... arg_types>
			value_type *add(const std::string &key, arg_types &&... args){
				exclusive_lock_type guard(lock_);
				auto entry = value_list_.try_emplace(key, std::make_shared<value_type>(target_type(std::forward<arg_types>(args)...)));
				if (!entry.second)//Failed to insert
					return nullptr;

				order_list_.push_front(std::make_pair(&entry.first->first, entry.first->second.get()));
				return entry.first->second.get();
			}

			virtual void remove(const std::string &key);

			virtual void remove(value_type *value);

			virtual void use(const std::string &key, value_ptr_type value);

			virtual void use(object &storage);

		protected:
			void use_(const std::string &key, value_ptr_type value);

			void remove_(value_list_iterator_type iter);

			value_list_iterator_type find_(const std::string &key);

			value_list_iterator_type find_(value_type *value);

			object *parent_;
			value_list_type value_list_;
			order_list_type order_list_;
			bool inside_destructor_;
		};
	}
}

#endif /* !OOSL_STORAGE_OBJECT_H */
