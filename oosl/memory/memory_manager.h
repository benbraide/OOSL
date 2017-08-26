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
#include "memory_tls_thunk.h"

namespace oosl{
	namespace memory{
		class manager{
		public:
			typedef block::attribute_type attribute_type;
			typedef block::uint64_type uint64_type;
			typedef block::size_type size_type;

			typedef std::unordered_map<uint64_type, block> block_list_type;
			typedef std::unordered_map<uint64_type, block *> block_ptr_list_type;
			typedef std::map<uint64_type, size_type> available_list_type;

			typedef std::shared_ptr<dependency> dependency_ptr_type;
			typedef std::unordered_map<uint64_type, dependency_ptr_type> dependency_list_type;

			typedef watcher::range_type watcher_range_type;
			typedef std::shared_ptr<watcher> watcher_ptr_type;

			typedef std::unordered_map<uint64_type, watcher_ptr_type> watchers_map_type;
			typedef std::map<watcher_range_type, watchers_map_type> watcher_list_type;

			typedef std::shared_ptr<tls_thunk> tls_thunk_ptr_type;
			typedef std::list<tls_thunk_ptr_type> tls_thunk_ptr_list_type;

			typedef std::thread thread_type;
			typedef thread_type::id thread_id_type;

			typedef std::shared_mutex lock_type;
			typedef common::lock_once<lock_type> lock_once_type;

			typedef std::mutex atomic_lock_type;
			typedef common::lock_once<atomic_lock_type> atomic_lock_once_type;

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

			void on_thread_entry();

			void on_thread_exit();

			void add_dependency(uint64_type address, dependency_ptr_type value);

			void remove_dependency(uint64_type address);

			uint64_type add_watcher(const watcher_range_type &range, watcher_ptr_type value);

			void remove_watcher(uint64_type id);

			void capture_tls(tls_thunk_ptr_type thunk);

			void deallocate(uint64_type address, deallocation_option options = deallocation_option::nil);

			uint64_type reserve(size_type size);

			template <typename target_type>
			block *allocate(uint64_type address = 0ull){
				auto entry = allocate(sizeof(target_type), address);
				if (std::is_arithmetic<target_type>::value){
					OOSL_SET(entry->attributes, attribute_type::numeric);
					if (std::is_floating_point<target_type>::value){
						OOSL_SET(entry->attributes, attribute_type::floating_point);
						if (std::is_same<target_type, long double>::value)
							OOSL_SET(entry->attributes, attribute_type::long_double);
					}
					else if (std::is_unsigned<target_type>::value)
						OOSL_SET(entry->attributes, attribute_type::unsigned_integer);
				}
				
				return entry;
			}

			block *allocate(size_type size, uint64_type address = 0ull);

			block *allocate_contiguous(size_type count, size_type size);

			template <typename value_type>
			block *allocate_scalar(value_type value){
				lock_once_type guard(lock_);
				auto entry = allocate(sizeof(value_type));

				OOSL_SET(entry->attributes, attribute_type::immutable);
				if (std::is_arithmetic<value_type>::value){
					OOSL_SET(entry->attributes, attribute_type::numeric);
					if (std::is_floating_point<value_type>::value){
						OOSL_SET(entry->attributes, attribute_type::floating_point);
						if (std::is_same<value_type, long double>::value)
							OOSL_SET(entry->attributes, attribute_type::long_double);
					}
					else if (std::is_unsigned<value_type>::value)
						OOSL_SET(entry->attributes, attribute_type::unsigned_integer);
				}

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

			block *atomic(uint64_type address);

			void fill(uint64_type address, char source, size_type count);

			void copy(uint64_type destination, uint64_type source, size_type size);

			template <typename value_type>
			void write(uint64_type address, value_type source){
				write_(address, reinterpret_cast<const char *>(&source), sizeof(value_type), true);
			}

			template <typename value_type>
			void write(uint64_type address, const value_type *source, size_type count){
				write_(address, (const char *)source, count * sizeof(value_type), true);
			}

			template <typename value_type>
			void write_numeric(uint64_type address, value_type value){
				lock_once_type guard(lock_, shared_locker);

				auto entry = find_block(address);
				if (entry == nullptr)//Invalid address
					throw error_codes_type::invalid_address;

				pre_write_(*entry);
				if (OOSL_IS(entry->attributes, attribute_type::floating_point)){
					switch (entry->size){
					case OOSL_MM_FLT_SIZE://float
						write_numeric_<float>(entry->ptr, value);
						break;
					case OOSL_MM_DBL_SIZE://double | long doub;e
						if (OOSL_IS(entry->attributes, attribute_type::long_double))
							write_numeric_<long double>(entry->ptr, value);
						else
							write_numeric_<double>(entry->ptr, value);
						break;
					default://Error
						throw error_codes_type::invalid_memory_size;
						break;
					}
				}
				else{//Integral
					switch (entry->size){
					case OOSL_MM_INT8_SIZE://8-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							write_numeric_<unsigned __int8>(entry->ptr, value);
						else//Signed
							write_numeric_<__int8>(entry->ptr, value);
						break;
					case OOSL_MM_INT16_SIZE://16-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							write_numeric_<unsigned __int16>(entry->ptr, value);
						else//Signed
							write_numeric_<__int16>(entry->ptr, value);
						break;
					case OOSL_MM_INT32_SIZE://32-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							write_numeric_<unsigned __int32>(entry->ptr, value);
						else//Signed
							write_numeric_<__int32>(entry->ptr, value);
						break;
					case OOSL_MM_INT64_SIZE://64-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							write_numeric_<unsigned __int64>(entry->ptr, value);
						else//Signed
							write_numeric_<__int64>(entry->ptr, value);
						break;
					default://Error
						throw error_codes_type::invalid_memory_size;
						break;
					}
				}

				call_watchers_(watcher_range_type{ address, address + entry->actual_size - 1 });
			}

			void read(uint64_type address, char *buffer, size_type size);

			template <typename target_type>
			target_type read(uint64_type address){
				auto value = target_type();
				read(address, reinterpret_cast<char *>(&value), sizeof(target_type));
				return value;
			}

			template <typename target_type>
			target_type read_numeric(uint64_type address){
				lock_once_type guard(lock_, shared_locker);

				auto entry = find_block(address);
				if (entry == nullptr)//Invalid address
					throw error_codes_type::invalid_address;

				if (OOSL_IS(entry->attributes, attribute_type::floating_point)){
					switch (entry->size){
					case OOSL_MM_FLT_SIZE://float
						return read_numeric_<target_type, float>(entry->ptr);
					case OOSL_MM_DBL_SIZE://double | long doub;e
						if (OOSL_IS(entry->attributes, attribute_type::long_double))
							return read_numeric_<target_type, long double>(entry->ptr);
						return read_numeric_<target_type, double>(entry->ptr);
					default://Error
						break;
					}
				}
				else{//Integral
					switch (entry->size){
					case OOSL_MM_INT8_SIZE://8-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							return read_numeric_<target_type, unsigned __int8>(entry->ptr);
						return read_numeric_<target_type, __int8>(entry->ptr);
					case OOSL_MM_INT16_SIZE://16-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							return read_numeric_<target_type, unsigned __int16>(entry->ptr);
						return read_numeric_<target_type, __int16>(entry->ptr);
					case OOSL_MM_INT32_SIZE://32-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							return read_numeric_<target_type, unsigned __int32>(entry->ptr);
						return read_numeric_<target_type, __int32>(entry->ptr);
					case OOSL_MM_INT64_SIZE://64-bit integer
						if (OOSL_IS(entry->attributes, attribute_type::unsigned_integer))
							return read_numeric_<target_type, unsigned __int64>(entry->ptr);
						return read_numeric_<target_type, __int64>(entry->ptr);
					default://Error
						break;
					}
				}

				throw error_codes_type::invalid_memory_size;
			}

			block *find_block(uint64_type address);

			block *find_enclosing_block(uint64_type address);

			watcher *find_watcher_by_id(uint64_type id);

			bool is_torn() const;

			bool is_protected_mode() const;

			bool is_protected(uint64_type address) const;

			lock_type &lock();

			static const shared_locker_type shared_locker;

		private:
			template <typename value_type>
			block *allocate_scalar_(const value_type *value, size_type count){
				lock_once_type guard(lock_);
				auto entry = allocate(count * sizeof(value_type));

				OOSL_SET(entry->attributes, attribute_type::immutable);
				str_cpy_(entry->ptr, (const char *)value, entry->size);

				return entry;
			}

			void pre_write_(block &memory_block);

			void write_(uint64_type address, const char *source, size_type size, bool is_array);

			template <typename target_type, typename value_type>
			void write_numeric_(char *destination, value_type value){
				auto converted = static_cast<target_type>(value);
				str_cpy_(destination, reinterpret_cast<const char *>(&converted), sizeof(target_type));
			}

			template <typename target_type, typename value_type>
			target_type read_numeric_(const char *source){
				auto value = value_type();
				str_cpy_(reinterpret_cast<char *>(&value), source, sizeof(value_type));
				return static_cast<target_type>(value);
			}

			void add_available_(uint64_type value, size_type size);

			uint64_type find_available_(size_type size, uint64_type match = 0ull);

			void call_watchers_(const watcher_range_type &range);

			void str_cpy_(char *destination, const char *source, size_type count);

			uint64_type protected_;
			uint64_type next_address_;

			block_list_type blocks_;
			tls_thunk_ptr_list_type tls_captures_;

			available_list_type available_list_;
			dependency_list_type dependencies_;
			watcher_list_type watchers_;

			state_type states_;
			random_engine_type random_engine_;

			lock_type lock_;
			atomic_lock_type atomic_lock_;

			static thread_local block_ptr_list_type tls_blocks_;
			static thread_local bool is_protected_;
		};

		OOSL_MAKE_OPERATORS(manager::state_type);
		OOSL_MAKE_OPERATORS(manager::deallocation_option);
	}
}

#endif /* !OOSL_MEMORY_MANAGER_H */
