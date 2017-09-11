#pragma once

#ifndef OOSL_TEMPORARY_STORAGE_H
#define OOSL_TEMPORARY_STORAGE_H

#include <list>

#include "storage_entry.h"

#include "../type/type_mapper.h"
#include "../memory/memory_manager.h"
#include "../common/controller.h"

namespace oosl{
	namespace storage{
		class temporary{
		public:
			typedef oosl::storage::entry entry_type;
			typedef entry_type::attribute_type entry_attribute_type;

			typedef oosl::type::object::ptr_type type_ptr_type;
			typedef std::list<entry_type> entry_list_type;

			virtual ~temporary();

			template <typename... args_types>
			entry_type *add(args_types &&... args){
				auto block = common::controller::active->memory().allocate(std::forward<args_types>(args)...);
				return &*entry_list_.emplace(entry_list_.end(), entry_type{
					nullptr,
					block->address
				});
			}

			template <typename value_type>
			entry_type *add_scalar(value_type value, type_ptr_type type = nullptr){
				auto block = common::controller::active->memory().allocate_scalar(value);
				if (type == nullptr)//Find type
					type = common::controller::active->find_type(oosl::type::mapper<value_type>::id);

				return &*entry_list_.emplace(entry_list_.end(), entry_type{
					nullptr,
					block->address,
					entry_attribute_type::nil,
					type
				});
			}

		protected:
			entry_list_type entry_list_;
		};
	}
}

#endif /* !OOSL_TEMPORARY_STORAGE_H */
