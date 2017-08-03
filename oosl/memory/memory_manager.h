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

			struct tls_info{
				uint64_type address;
				char *ptr;
			};

			typedef std::unordered_map<uint64_type, block> block_list_type;
			typedef std::map<uint64_type, available_info> available_list_type;

			typedef std::list<tls_info> tls_list_type;

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

			explicit manager(uint64_type protected_range = 0ull);

			~manager();

			static const shared_locker_type shared_locker;

		private:
			uint64_type next_;
			uint64_type protected_;

			block_list_type blocks_;
			tls_list_type tls_captures_;

			available_list_type available_list_;
			dependency_list_type dependencies_;
			watcher_list_type watchers_;

			lock_type lock_;

			static thread_local block_list_type tls_blocks_;
			static thread_local watcher_list_type tls_watchers_;
		};
	}
}

#endif /* !OOSL_MEMORY_MANAGER_H */
