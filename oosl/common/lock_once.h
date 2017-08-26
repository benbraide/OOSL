#pragma once

#ifndef OOSL_LOCK_ONCE_H
#define OOSL_LOCK_ONCE_H

#include <cstdlib>

namespace oosl{
	namespace common{
		struct lock_once_info{
			static thread_local bool lock_is_acquired;
		};

		template <class lock_type>
		class lock_once{
		public:
			typedef lock_type lock_type;

			explicit lock_once(lock_type &lock)
				: lock_once(&lock){}

			explicit lock_once(lock_type *lock)
				: lock_(nullptr){
				if (lock != nullptr && !lock_once_info::lock_is_acquired){
					lock_once_info::lock_is_acquired = true;
					(lock_ = lock)->lock();
				}
			}

			template <typename function_type, typename... args_types>
			lock_once(lock_type &lock, function_type locker, args_types &&... args)
				: lock_once(&lock, locker, std::forward<args_types>(args)...){}

			template <typename function_type, typename... args_types>
			lock_once(lock_type *lock, function_type locker, args_types &&... args)
				: lock_(nullptr){
				if (lock != nullptr && !lock_once_info::lock_is_acquired){
					lock_once_info::lock_is_acquired = true;
					((lock_ = lock)->*locker)(std::forward<args_types>(args)...);
				}
			}

			~lock_once(){
				unlock();
			}

			void unlock(){
				if (lock_ == nullptr)
					return;//No lock acquired
				
				if (lock_once_info::lock_is_acquired){//Release lock
					lock_->unlock();
					lock_once_info::lock_is_acquired = false;
				}
				
				lock_ = nullptr;//Reset
			}

		private:
			lock_type *lock_;
		};
	}
}

#endif /* !OOSL_LOCK_ONCE_H */
