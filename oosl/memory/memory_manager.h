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

#include "memory_dependency.h"
#include "memory_watcher.h"

namespace oosl{
	namespace memory{
		class manager{
		public:
			typedef block::attribute_type attribute_type;
			typedef block::uint64_type uint64_type;
			typedef block::size_type size_type;

			struct available_info{
				size_type size;
				char *ptr;
			};

			typedef std::unordered_map<uint64_type, block> block_list_type;
			typedef std::map<uint64_type, available_info> available_list_type;

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

			enum class state_type : unsigned int{
				nil					= (0 << 0x0000),
				torn				= (1 << 0x0000),
				protected_mode		= (1 << 0x0001),
			};

			explicit manager(uint64_type protected_range = 0ull);

			~manager();

			void tear();

			void enter_protected_mode();

			void leave_protected_mode();

			void on_thread_exit();

			void capture_tls(uint64_type address, block *memory_block);

			block *find_block(uint64_type address);

			block *find_enclosing_block(uint64_type address);

			bool is_torn() const;

			bool is_protected_mode() const;

			bool is_protected(uint64_type address) const;

			static const shared_locker_type shared_locker;

		private:
			block *initialize_tls_(block &memory_block);

			block *find_enclosing_block_(block_list_type &blocks, uint64_type address);

			uint64_type protected_;
			uint64_type next_address_;

			block_list_type *blocks_;
			tls_list_type tls_captures_;

			available_list_type available_list_;
			dependency_list_type dependencies_;
			watcher_list_type watchers_;

			state_type states_;
			lock_type lock_;

			static thread_local block_list_type tls_blocks_;
			static thread_local watcher_list_type tls_watchers_;

			static thread_local bool is_protected_;
		};

		OOSL_MAKE_OPERATORS(manager::state_type);
	}
}

#endif /* !OOSL_MEMORY_MANAGER_H */
