#include "memory_manager.h"

oosl::memory::manager::manager(uint64_type protected_range)
	: protected_(protected_range), next_address_(protected_range + 1), states_(state_type::nil){}

oosl::memory::manager::~manager(){
	lock_once_type guard(lock_);
	if (!is_torn()){
		if (!blocks_.empty()){
			for (auto &entry : blocks_){//Free all allocated memory
				if (entry.second.ptr != nullptr)
					delete[] entry.second.ptr;
			}
		}
	}
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

void oosl::memory::manager::on_thread_entry(){
	lock_once_type guard(lock_);
	if (is_torn())
		return;

	block *entry;
	for (auto thunk : tls_captures_){//Allocate thread local memory
		if ((entry = thunk->execute(*this)) == nullptr)
			throw error_codes_type::tls_error;
		else//Add the tls attribute
			OOSL_SET(entry->attributes, attribute_type::tls);
	}
}

void oosl::memory::manager::on_thread_exit(){
	lock_once_type guard(lock_);
	if (is_torn())
		return;

	auto &blocks = tls_blocks_;
	for (auto &entry : blocks)//Free thread local storage
		deallocate(entry.second->address, deallocation_option::no_throw);
}

void oosl::memory::manager::add_dependency(uint64_type address, dependency_ptr_type value){
	lock_once_type guard(lock_);
	dependencies_[address] = value;
}

void oosl::memory::manager::remove_dependency(uint64_type address){
	lock_once_type guard(lock_);
	dependencies_.erase(address);
}

oosl::memory::manager::uint64_type oosl::memory::manager::add_watcher(const watcher_range_type &range, watcher_ptr_type value){
	if (is_torn())
		return 0ull;

	lock_once_type guard(lock_);
	auto id = random_engine_(1ull, std::numeric_limits<uint64_type>::max());
	if (find_watcher_by_id(id) != nullptr)
		return 0ull;

	watchers_[range][id] = value;
	return id;
}

void oosl::memory::manager::remove_watcher(uint64_type id){
	if (is_torn())
		return;

	lock_once_type guard(lock_);
	for (auto entry = watchers_.begin(); entry != watchers_.end(); ++entry){
		auto id_entry = entry->second.find(id);
		if (id_entry != entry->second.end()){
			entry->second.erase(id_entry);
			if (entry->second.empty())//Remove empty list
				watchers_.erase(entry);
			break;
		}
	}
}

void oosl::memory::manager::capture_tls(tls_thunk_ptr_type thunk){
	lock_once_type guard(lock_);
	if (!is_torn())
		tls_captures_.push_back(thunk);
}

void oosl::memory::manager::deallocate(uint64_type address, deallocation_option options){
	if (is_torn())
		return;

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
		deallocate(*reinterpret_cast<uint64_type *>(entry->ptr), deallocation_option::no_throw);//Deallocate linked memory

	if (!OOSL_IS(options, deallocation_option::no_merge))//Add to available list
		add_available_(address, entry->actual_size);

	if (entry->ptr != nullptr && !OOSL_IS(entry->attributes, attribute_type::not_allocated))
		delete[] entry->ptr;//Free memory

	watchers_.erase(watcher_range_type{ entry->address, (entry->address + entry->actual_size - 1) });
	dependencies_.erase(address);

	blocks_.erase(address);
	if (OOSL_IS(entry->attributes, attribute_type::tls))
		tls_blocks_.erase(address);
}

oosl::memory::manager::uint64_type oosl::memory::manager::reserve(size_type size){
	if (is_torn())
		return 0ull;

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
	if (is_torn())
		return nullptr;

	lock_once_type guard(lock_);
	if (blocks_.max_size() <= blocks_.size())
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

	return &(blocks_[address] = block{ 1u, address, size, actual_size, attribute_type::nil, ptr });
}

oosl::memory::block *oosl::memory::manager::allocate_contiguous(size_type count, size_type size){
	if (is_torn())
		return nullptr;

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
	if (size == 0u || is_torn())
		return nullptr;

	lock_once_type guard(lock_);
	auto entry = find_block(address);
	if (entry == nullptr)
		throw error_codes_type::invalid_address;

	if (entry->size < size){//Expand
		if (entry->actual_size < size){//Reallocate
			auto old_ptr = entry->ptr;
			auto attributes = entry->attributes;
			auto old_size = entry->size;

			entry->ptr = nullptr;//Preserve memory
			deallocate(address);

			entry = allocate(size);
			entry->attributes = attributes;//Restore attributes

			if (old_ptr != nullptr){//Duplicate and free memory
				memcpy_s(entry->ptr, entry->actual_size, old_ptr, old_size);
				delete[] old_ptr;
			}
		}
		else//Expand size
			entry->size = size;
	}
	else if (size < entry->size)//Shrink
		entry->size = size;

	return entry;
}

oosl::memory::block *oosl::memory::manager::atomic(uint64_type address){
	lock_once_type guard(lock_);

	auto entry = allocate_scalar(address);
	if (entry != nullptr)//Set atomic attribute
		OOSL_SET(entry->attributes, attribute_type::atomic);

	return entry;
}

void oosl::memory::manager::fill(uint64_type address, char source, size_type count){
	write_(address, &source, count, false);
}

void oosl::memory::manager::copy(uint64_type destination, uint64_type source, size_type size){
	lock_once_type guard(lock_, shared_locker);

	block *entry = nullptr;
	size_type available_size = 0u, target_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((entry = (entry == nullptr) ? find_enclosing_block(source) : find_block(source)) != nullptr){
			ptr_index = static_cast<size_type>(source - entry->address);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			throw common::error_codes::read_violation;

		target_size = (available_size < size) ? available_size : size;
		write_(destination, entry->ptr + ptr_index, target_size, true);

		source += target_size;
		destination += target_size;
		size -= target_size;
	}
}

void oosl::memory::manager::read(uint64_type address, char *buffer, size_type size){
	lock_once_type guard(lock_, shared_locker);

	block *entry = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((entry = (entry == nullptr) ? find_enclosing_block(address) : find_block(address)) != nullptr){//Get next block
			ptr_index = static_cast<size_type>(address - entry->address);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			throw common::error_codes::read_violation;

		min_size = (available_size < size) ? available_size : size;
		memcpy_s(buffer, size, entry->ptr, min_size);//Read block

		buffer += min_size;
		address += min_size;
		size -= min_size;
	}
}

oosl::memory::block *oosl::memory::manager::find_block(uint64_type address){
	if (is_torn())
		return nullptr;

	lock_once_type guard(lock_, shared_locker);//Acquire general lock
	if (address <= protected_ && !is_protected_mode())
		throw error_codes_type::read_violation;

	auto entry = blocks_.find(address);
	return (entry == blocks_.end()) ? nullptr : &entry->second;
}

oosl::memory::block *oosl::memory::manager::find_enclosing_block(uint64_type address){
	if (is_torn())
		return nullptr;

	lock_once_type guard(lock_, shared_locker);//Acquire general lock
	if (address <= protected_ && !is_protected_mode())
		throw error_codes_type::read_violation;

	block *block_entry = nullptr;
	for (auto &entry : blocks_){
		if (entry.first == address || (entry.first < address && address < (entry.first + entry.second.actual_size))){
			block_entry = &entry.second;
			break;
		}
	}

	return block_entry;
}

oosl::memory::watcher *oosl::memory::manager::find_watcher_by_id(uint64_type id){
	for (auto &entry : watchers_){
		auto id_entry = entry.second.find(id);
		if (id_entry != entry.second.end())
			return id_entry->second.get();
	}

	return nullptr;
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

oosl::memory::manager::lock_type &oosl::memory::manager::lock(){
	return lock_;
}

const oosl::memory::manager::shared_locker_type oosl::memory::manager::shared_locker = &lock_type::lock_shared;

void oosl::memory::manager::pre_write_(block &memory_block){
	if (OOSL_IS(memory_block.attributes, attribute_type::immutable))
		throw common::error_codes::write_violation;//Write violation

	if (memory_block.ptr != nullptr && OOSL_IS(memory_block.attributes, attribute_type::indirect))//Deallocate linked memory
		deallocate(*reinterpret_cast<uint64_type *>(memory_block.ptr), deallocation_option::no_throw);
}

void oosl::memory::manager::write_(uint64_type address, const char *source, size_type size, bool is_array){
	lock_once_type guard(lock_, shared_locker);
	if (size == 0u)
		return;

	block *entry = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;
	watcher *watcher = nullptr;

	while (size > 0u){
		if (entry == nullptr){//First block
			if ((entry = find_enclosing_block(address)) != nullptr && entry->address == address && OOSL_IS(entry->attributes, attribute_type::atomic)){//Atomic write
				atomic_lock_once_type guard(atomic_lock_);//Acquire atomic lock
				write_(read_numeric_<uint64_type, uint64_type>(entry->ptr), source, size, is_array);
				return;
			}
		}
		else//Continue with next block
			entry = find_block(address);

		if (entry != nullptr){//Valid block
			ptr_index = static_cast<size_type>(address - entry->address);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			throw common::error_codes::read_violation;

		pre_write_(*entry);
		min_size = (available_size < size) ? available_size : size;
		if (is_array)
			memcpy_s(entry->ptr + ptr_index, size, source, min_size);
		else//Set applicable
			std::memset(entry->ptr + ptr_index, *source, min_size);

		call_watchers_(watcher_range_type{ address, address + available_size - 1 });
		if (is_array)//Advance source pointer
			source += min_size;

		address += min_size;
		size -= min_size;
	}
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

void oosl::memory::manager::call_watchers_(const watcher_range_type &range){
	for (auto &entry : watchers_){
		if (entry.first.contains(range)){//Alert
			for (auto &id_entry : entry.second)
				id_entry.second->on_change(entry.first, range);
		}
		else if (range.left() < entry.first.left())
			break;
	}
}

void oosl::memory::manager::str_cpy_(char *destination, const char *source, size_type count){
	memcpy_s(destination, count, source, count);
}

thread_local oosl::memory::manager::block_ptr_list_type oosl::memory::manager::tls_blocks_;

thread_local bool oosl::memory::manager::is_protected_ = false;
