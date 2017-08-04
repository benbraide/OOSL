#include "memory_manager.h"

#pragma warning(disable: 4996)

oosl::memory::manager::manager(uint64_type protected_range)
	: protected_(protected_range), next_address_(protected_range + 1), blocks_(&tls_blocks_), states_(state_type::nil){}

oosl::memory::manager::~manager(){
	tear();
}

void oosl::memory::manager::tear(){
	OOSL_SET(states_, state_type::torn);
}

void oosl::memory::manager::enter_protected_mode(){
	is_protected_ = true;
}

void oosl::memory::manager::leave_protected_mode(){
	is_protected_ = false;
}

void oosl::memory::manager::on_thread_exit(){
	if (is_torn())
		return;
}

void oosl::memory::manager::capture_tls(uint64_type address, block *memory_block){
	if (is_torn())
		return;

	lock_once_type guard(lock_);
	if (memory_block == nullptr){//Find block
		auto entry = blocks_->find(address);
		if (entry != blocks_->end())
			memory_block = &entry->second;
	}

	if (memory_block != nullptr){
		auto tls_entry = &(tls_captures_[memory_block->address] = *memory_block);
		if (memory_block->ptr != nullptr){//Duplicate bytes
			tls_entry->ptr = new char[tls_entry->actual_size];
			std::strncpy(tls_entry->ptr, memory_block->ptr, tls_entry->actual_size);
		}
	}
}

oosl::memory::block *oosl::memory::manager::find_block(uint64_type address){
	if (is_torn())
		return nullptr;

	auto &tls_blocks = tls_blocks_;
	auto entry = tls_blocks.find(address);
	if (entry != tls_blocks.end())//Entry found in thread list
		return &entry->second;

	if (blocks_ == &tls_blocks)//Main list is same as this thread list
		return nullptr;

	lock_once_type guard(lock_, shared_locker);//Acquire general lock
	if ((entry = tls_captures_.find(address)) != tls_captures_.end())
		return initialize_tls_(entry->second);//Uninitialized tls

	return ((entry = blocks_->find(address)) == blocks_->end()) ? nullptr : &entry->second;
}

oosl::memory::block *oosl::memory::manager::find_enclosing_block(uint64_type address){
	if (is_torn())
		return nullptr;

	auto entry = find_enclosing_block_(tls_blocks_, address);
	if (entry != nullptr)
		return entry;

	if (blocks_ == &tls_blocks_)//Main list is same as this thread list
		return nullptr;

	lock_once_type guard(lock_, shared_locker);//Acquire general lock
	if ((entry = find_enclosing_block_(tls_captures_, address)) != nullptr)
		return initialize_tls_(*entry);

	return find_enclosing_block_(*blocks_, address);
}

bool oosl::memory::manager::is_torn() const{
	return OOSL_IS(states_, state_type::torn);
}

bool oosl::memory::manager::is_protected_mode() const{
	return is_protected_;
}

bool oosl::memory::manager::is_protected(uint64_type address) const{
	return (address <= protected_);
}

const oosl::memory::manager::shared_locker_type oosl::memory::manager::shared_locker = &lock_type::lock_shared;

oosl::memory::block *oosl::memory::manager::initialize_tls_(block &memory_block){
	auto tls_entry = &tls_blocks_[memory_block.address];

	*tls_entry = memory_block;
	if (memory_block.ptr != nullptr){//Duplicate bytes
		tls_entry->ptr = new char[tls_entry->actual_size];
		std::strncpy(tls_entry->ptr, memory_block.ptr, tls_entry->actual_size);
	}

	return tls_entry;
}

oosl::memory::block *oosl::memory::manager::find_enclosing_block_(block_list_type &blocks, uint64_type address){
	block *block_entry = nullptr;
	for (auto &entry : blocks){
		if (entry.first == address || (entry.first < address && address < (entry.first + entry.second.actual_size))){
			block_entry = &entry.second;
			break;
		}
	}

	return block_entry;
}

thread_local oosl::memory::manager::block_list_type oosl::memory::manager::tls_blocks_;

thread_local oosl::memory::manager::watcher_list_type oosl::memory::manager::tls_watchers_;

thread_local bool oosl::memory::manager::is_protected_ = false;
