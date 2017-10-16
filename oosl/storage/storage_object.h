#pragma once

#ifndef OOSL_STORAGE_OBJECT_H
#define OOSL_STORAGE_OBJECT_H

#include <list>
#include <unordered_map>

#include "storage_entry.h"
#include "storage_value.h"

#include "../memory/memory_manager.h"

namespace oosl{
	namespace node{
		class object;
	}

	namespace storage{
		class storage_tls_entry : public oosl::common::tls_entry{
		public:
			typedef oosl::storage::value value_type;

			typedef std::shared_ptr<value_type> value_ptr_type;
			typedef std::unordered_map<std::string, value_ptr_type> value_list_type;

			virtual value_type *find(const std::string &key);

			virtual value_type *add(const std::string &key, value_ptr_type value);

		private:
			value_list_type value_list_;
		};

		template <class node_type>
		class entry_value : public value{
		public:
			typedef node_type node_type;

			template <typename... args_type>
			explicit entry_value(node_type node, args_type &&... args)
				: value(std::forward<args_type>(args)...), node_(node){}

			node_type node(){
				return node_;
			}

		protected:
			node_type node_;
		};

		class object{
		public:
			typedef unsigned __int64 uint64_type;

			typedef oosl::storage::entry entry_type;
			typedef oosl::storage::value value_type;

			typedef std::shared_ptr<value_type> value_ptr_type;
			typedef std::unordered_map<std::string, value_ptr_type> value_list_type;
			typedef value_list_type::iterator value_list_iterator_type;

			typedef std::pair<const std::string *, value_type *> order_info_type;
			typedef std::list<order_info_type> order_list_type;

			typedef oosl::node::object node_type;
			typedef std::shared_ptr<node_type> node_ptr_type;

			typedef std::shared_mutex lock_type;
			typedef std::lock_guard<lock_type> guard_type;
			typedef std::shared_lock<lock_type> shared_guard_type;

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
				guard_type guard(lock_);
				if (value_list_.find(key) != value_list_.end())
					throw common::error_codes::redefinition;

				return add_<target_type>(key, std::forward<arg_types>(args)...);
			}

			value_type *add(const std::string &key, entry_type &entry, node_ptr_type node);

			virtual void remove(const std::string &key);

			virtual void remove(value_type *value);

			virtual void use(const std::string &key, value_ptr_type value);

			virtual void use(object &storage);

		protected:
			virtual void use_(const std::string &key, value_ptr_type value);

			template <typename target_type, typename... arg_types>
			value_type *add_(const std::string &key, arg_types &&... args){
				auto entry = value_list_.try_emplace(key, std::make_shared<target_type>(std::forward<arg_types>(args)...));
				if (!entry.second)//Failed to insert
					throw common::error_codes::out_of_memory;

				order_list_.push_front(std::make_pair(&entry.first->first, entry.first->second.get()));
				return entry.first->second.get();
			}

			virtual void remove_(value_list_iterator_type iter);

			virtual value_type *do_find_(const std::string &key, find_type type);

			virtual value_type *find_locked_(const std::string &key);

			virtual value_type *find_(const std::string &key);

			virtual value_list_iterator_type find_(value_type *value);

			object *parent_;
			value_list_type value_list_;
			order_list_type order_list_;
			bool inside_destructor_;
			lock_type lock_;
		};
	}
}

#endif /* !OOSL_STORAGE_OBJECT_H */
