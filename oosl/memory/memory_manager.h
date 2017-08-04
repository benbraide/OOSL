#pragma once

#ifndef OOSL_MEMORY_MANAGER_H
#define OOSL_MEMORY_MANAGER_H

#include <thread>
#include <shared_mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include <map>
#include <climits>
#include <vector>

#include "../common/lock_once.h"
#include "../common/error_codes.h"
#include "../common/random_number.h"

#include "memory_dependency.h"
#include "memory_watcher.h"

namespace oosl{
	namespace memory{
		class manager{
		public:
			typedef block::attribute_type attribute_type;
			typedef block::uint64_type uint64_type;
			typedef block::size_type size_type;

			typedef std::unordered_map<uint64_type, block> block_list_type;
			typedef std::map<uint64_type, size_type> available_list_type;

			typedef std::unordered_map<uint64_type, block> tls_list_type;

			typedef std::shared_ptr<dependency> dependency_ptr_type;
			typedef std::unordered_map<uint64_type, dependency_ptr_type> dependency_list_type;

			typedef watcher::range_type watcher_range_type;
			typedef std::shared_ptr<watcher> watcher_ptr_type;

			typedef std::unordered_map<uint64_type, watcher_ptr_type> watchers_map_type;
			typedef std::map<watcher_range_type, watchers_map_type> watcher_list_type;

			typedef std::thread thread_type;
			typedef thread_type::id thread_id_type;

			typedef std::shared_mutex lock_type;
			typedef common::lock_once<lock_type> lock_once_type;

			typedef decltype(&lock_type::lock_shared) shared_locker_type;

			typedef common::error_codes error_codes_type;
			typedef common::basic_random_number<uint64_type> random_engine_type;

			enum class state_type : unsigned int{
				nil					= (0 << 0x0000),
				torn				= (1 << 0x0000),
				protected_mode		= (1 << 0x0001),
			};

			enum class deallocation_option : unsigned int{
				nil					= (0 << 0x0000),
				no_merge			= (1 << 0x0000),
				no_throw			= (1 << 0x0001),
			};

			explicit manager(uint64_type protected_range = 0ull);

			~manager();

			void tear();

			void enter_protected_mode();

			void leave_protected_mode();

			void on_thread_exit();

			uint64_type add_watcher(const watcher_range_type &range, watcher_ptr_type value);

			void remove_watcher(uint64_type id);

			void capture_tls(uint64_type address, block *memory_block);

			void deallocate(uint64_type address, deallocation_option options = deallocation_option::nil);

			uint64_type reserve(size_type size);

			block *allocate(size_type size, uint64_type address = 0ull);

			block *allocate_contiguous(size_type count, size_type size);

			template <typename value_type>
			block *allocate_scalar(value_type value){
				lock_once_type guard(lock_);
				auto entry = allocate(sizeof(value_type));

				OOSL_SET(entry->attributes, attribute_type::immutable);
				str_cpy_(entry->ptr, reinterpret_cast<const char *>(&value), entry->size);

				return entry;
			}

			template <typename value_type>
			block *allocate_scalar(const value_type *value, size_type count){
				return allocate_scalar_(value, count);
			}

			block *allocate_scalar(const char *value, size_type count = 0u);

			block *allocate_scalar(const wchar_t *value, size_type count = 0u);

			block *reallocate(uint64_type address, size_type size);

			block *find_block(uint64_type address);

			block *find_enclosing_block(uint64_type address);

			watcher *find_watcher_by_id(uint64_type id);

			bool is_torn() const;

			bool is_protected_mode() const;

			bool is_protected(uint64_type address) const;

			static const shared_locker_type shared_locker;

		private:
			block *allocate_(size_type size, uint64_type address, bool tls);

			template <typename value_type>
			block *allocate_scalar_(const value_type *value, size_type count){
				lock_once_type guard(lock_);
				auto entry = allocate(count * sizeof(value_type));

				OOSL_SET(entry->attributes, attribute_type::immutable);
				str_cpy_(entry->ptr, (const char *)value, entry->size);

				return entry;
			}

			block *initialize_tls_(block &memory_block);

			block *find_enclosing_block_(block_list_type &blocks, uint64_type address);

			void pre_write_(block &memory_block);

			void write_(uint64_type address, const char *source, size_type size, bool is_array);

			void add_available_(uint64_type value, size_type size);

			uint64_type find_available_(size_type size, uint64_type match = 0ull);

			void call_watchers_(const watcher_range_type &range);

			void str_cpy_(char *destination, const char *source, size_type count);

			uint64_type protected_;
			uint64_type next_address_;

			block_list_type *blocks_;
			tls_list_type tls_captures_;

			available_list_type available_list_;
			dependency_list_type dependencies_;
			watcher_list_type watchers_;

			state_type states_;
			lock_type lock_;
			random_engine_type random_engine_;

			static thread_local block_list_type tls_blocks_;
			static thread_local bool is_protected_;
		};

		OOSL_MAKE_OPERATORS(manager::state_type);
		OOSL_MAKE_OPERATORS(manager::deallocation_option);
	}
}

#endif /* !OOSL_MEMORY_MANAGER_H */
