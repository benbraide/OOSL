#include "memory_manager.h"

oosl::memory::manager::manager(uint64_type protected_range)
	: protected_(protected_range){

}

oosl::memory::manager::~manager(){

}

const oosl::memory::manager::shared_locker_type oosl::memory::manager::shared_locker = &lock_type::lock_shared;

thread_local oosl::memory::manager::block_list_type oosl::memory::manager::tls_blocks_;

thread_local oosl::memory::manager::watcher_list_type oosl::memory::manager::tls_watchers_;
