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

void oosl::memory::manager::deallocate(uint64_type address, deallocation_option options){
	lock_once_type guard(lock_);

	auto entry = find_block(address);
	if (entry == nullptr){//Failed to locate entry
		if (OOSL_IS(options, deallocation_option::no_throw))
			return;//Don't throw an exception
		throw error_codes_type::invalid_address;
	}

	if (entry->ref_count > 0u && --entry->ref_count > 0u)
		return;//Referenced by some other object

	if (entry->ptr != nullptr && OOSL_IS(entry->attributes, attribute_type::indirect))
		deallocate(*reinterpret_cast<uint64_type *>(entry->ptr));//Deallocate linked memory

	if (!OOSL_IS(options, deallocation_option::no_merge))//Add to available list
		add_available_(address, entry->actual_size);

	if (entry->ptr != nullptr && OOSL_IS(entry->attributes, attribute_type::allocated))
		delete[] entry->ptr;//Free memory

	watchers_.erase(watcher_range_type{ entry->address, (entry->address + entry->actual_size - 1) });
	dependencies_.erase(address);

	if (OOSL_IS(entry->attributes, attribute_type::tls))
		tls_blocks_.erase(address);
	else//Remove from main storage
		blocks_->erase(address);
}

oosl::memory::manager::uint64_type oosl::memory::manager::reserve(size_type size){
	lock_once_type guard(lock_);
	if (size == 0u)
		return 0ull;

	auto value = find_available_(size);
	if (value == 0ull){//Use next value
		if ((std::numeric_limits<uint64_type>::max() - size) < value)
			throw error_codes_type::out_of_memory;//Out of address space

		value = next_address_;
		next_address_ += size;
	}
	else//Remove from list
		available_list_.erase(value);

	return value;
}

oosl::memory::block *oosl::memory::manager::allocate(size_type size, uint64_type address){
	lock_once_type guard(lock_);
	if (blocks_->max_size() <= blocks_->size())
		throw error_codes_type::out_of_memory;//Out of address space

	auto merge = false;
	auto actual_size = ((size == 0u) ? 1u : size);

	if (address == 0ull){//Determine value
		merge = true;
		if ((address = find_available_(actual_size)) == 0ull){//Use next value
			if ((std::numeric_limits<uint64_type>::max() - actual_size) < address)
				throw error_codes_type::out_of_memory;//Out of address space

			address = next_address_;
			next_address_ += actual_size;
		}
		else//Remove from list
			available_list_.erase(address);
	}
	else if ((!is_protected_mode() && address <= protected_) || find_enclosing_block(address) != nullptr)
		throw error_codes_type::read_violation;

	auto ptr = new char[actual_size];
	if (ptr == nullptr){//Failed to allocate buffer
		if (merge)//Return address to available list
			add_available_(address, actual_size);
		throw error_codes_type::out_of_memory;
	}

	return &((*blocks_)[address] = block{ 1u, address, size, actual_size, attribute_type::nil, ptr });
}

oosl::memory::block *oosl::memory::manager::allocate_contiguous(size_type count, size_type size){
	lock_once_type guard(lock_);
	if (count == 0u)
		return nullptr;

	auto actual_size = ((size == 0u) ? 1u : size);
	auto address = reserve(actual_size * count);

	auto start_value = address;
	std::vector<block *> entries(size);

	for (auto i = 0u; i < count; ++i, address += actual_size){
		try{//Guard allocation
			entries[i] = allocate(size, address);
		}
		catch (error_codes_type){
			for (auto j = 0u; j < i; ++j)//Rollback
				deallocate(entries[j]->address, deallocation_option::no_merge);
			add_available_(start_value, actual_size * count);
			throw;//Forward exception
		}
	}

	return *entries.begin();
}

oosl::memory::block *oosl::memory::manager::allocate_scalar(const char *value, size_type count){
	return allocate_scalar_(value, (count == 0u) ? static_cast<size_type>(std::strlen(value) + 1) : count);
}

oosl::memory::block *oosl::memory::manager::allocate_scalar(const wchar_t *value, size_type count){
	return allocate_scalar_(value, (count == 0u) ? static_cast<size_type>(std::wcslen(value) + 1) : count);
}

oosl::memory::block *oosl::memory::manager::reallocate(uint64_type address, size_type size){
	lock_once_type guard(lock_);

	auto entry = find_block(address);
	if (entry == nullptr)
		throw error_codes_type::invalid_address;

	if (entry->size < size){//Expand
		if (entry->actual_size < size){//Reallocate
			auto attributes = entry->attributes;
			deallocate(address);

			entry = allocate(size);
			entry->attributes = attributes;//Restore attributes
		}
		else//Expand size
			entry->size = size;
	}
	else if (size < entry->size)//Shrink
		entry->size = size;

	return entry;
}

oosl::memory::block *oosl::memory::manager::find_block(uint64_type address){
	if (is_torn())
		return nullptr;

	if (address <= protected_ && !is_protected_mode())
		throw error_codes_type::read_violation;

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

	if (address <= protected_ && !is_protected_mode())
		throw error_codes_type::read_violation;

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

void oosl::memory::manager::pre_write_(block &memory_block){
	if (OOSL_IS(memory_block.attributes, attribute_type::immutable))
		throw common::error_codes::write_violation;//Write violation

	if (memory_block.ptr != nullptr && OOSL_IS(memory_block.attributes, attribute_type::indirect))
		deallocate(*reinterpret_cast<uint64_type *>(memory_block.ptr));//Deallocate linked memory
}

void oosl::memory::manager::add_available_(uint64_type value, size_type size){
	if (size == 0u)
		return;

	auto entry = available_list_.begin();
	for (; entry != available_list_.end(); ++entry){
		if ((entry->first + entry->second) == value)
			break;//Previous in sequence
	}

	if (entry != available_list_.end()){//Merge with previous
		entry->second += size;
		value = entry->first;
		size = entry->second;
	}

	auto next = available_list_.find(value + size);
	if (next != available_list_.end()){//Merge with next
		entry->second += next->second;
		available_list_.erase(next);
	}
	else//Add entry
		available_list_[value] = size;

	if (!available_list_.empty()){
		auto last = std::prev(available_list_.end());
		if ((last->first + last->second) == next_address_){//Move next address backwards
			next_address_ = last->first;
			available_list_.erase(last);
		}
	}
}

oosl::memory::manager::uint64_type oosl::memory::manager::find_available_(size_type size, uint64_type match){
	for (auto &entry : available_list_){
		if (match != 0ull && entry.first != match){
			if (match < entry.first)
				break;
			continue;
		}

		if (size < entry.second){//Use required
			available_list_[entry.first + size] = (entry.second - size);
			return entry.first;
		}

		if (size == entry.second)
			return entry.first;
	}

	return 0ull;
}

void oosl::memory::manager::str_cpy_(char *destination, const char *source, size_type count){
	std::strncpy(destination, source, count);
}

thread_local oosl::memory::manager::block_list_type oosl::memory::manager::tls_blocks_;

thread_local oosl::memory::manager::watcher_list_type oosl::memory::manager::tls_watchers_;

thread_local bool oosl::memory::manager::is_protected_ = false;
