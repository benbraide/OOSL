#pragma once

#ifndef OOSL_TEMPORARY_STORAGE_H
#define OOSL_TEMPORARY_STORAGE_H

#include <list>

#include "storage_entry.h"

#include "../type/type_mapper.h"
#include "../memory/memory_manager.h"
#include "../common/controller.h"
#include "../common/error_codes.h"

namespace oosl{
	namespace storage{
		class temporary{
		public:
			typedef unsigned __int64 uint64_type;

			typedef oosl::storage::entry entry_type;
			typedef entry_type::attribute_type entry_attribute_type;

			typedef oosl::type::bool_type bool_type;
			typedef oosl::type::object::ptr_type type_ptr_type;
			typedef oosl::type::id type_id_type;

			typedef std::list<entry_type> entry_list_type;
			typedef oosl::memory::block::attribute_type memory_attribute_type;

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
				return add_scalar_(value, type);
			}

			entry_type *add_scalar(bool_type value);

			entry_type *add_scalar(const std::string &value);

			entry_type *add_scalar(const std::wstring &value);

			entry_type *wrap(uint64_type address, type_ptr_type type, entry_attribute_type attributes = entry_attribute_type::nil);

		protected:
			template <typename value_type>
			entry_type *add_scalar_(value_type value, type_ptr_type type){
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

			template <typename string_type>
			entry_type *add_string_scalar_(const string_type &value, type_ptr_type type){
				typedef typename string_type::traits_type::char_type char_type;

				auto string_block = oosl::common::controller::active->memory().allocate(static_cast<std::size_t>((value.size() + 1) * sizeof(char_type)));
				if (string_block == nullptr)//Error
					throw common::error_codes::out_of_memory;

				auto holder_block = oosl::common::controller::active->memory().allocate_scalar(string_block->address);
				if (holder_block == nullptr)//Error
					throw common::error_codes::out_of_memory;

				memcpy(string_block->ptr, value.c_str(), string_block->size);
				OOSL_SET(string_block->attributes, memory_attribute_type::immutable);
				OOSL_SET(holder_block->attributes, memory_attribute_type::indirect);

				return &*entry_list_.emplace(entry_list_.end(), entry_type{
					nullptr,
					holder_block->address,
					entry_attribute_type::nil,
					type
				});
			}

			entry_list_type entry_list_;
		};
	}
}

#endif /* !OOSL_TEMPORARY_STORAGE_H */
