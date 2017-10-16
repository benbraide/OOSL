#pragma once

#ifndef OOSL_TLS_H
#define OOSL_TLS_H

#include <memory>
#include <unordered_map>

namespace oosl{
	namespace common{
		class tls_entry{
		public:
			virtual ~tls_entry();
		};

		class tls{
		public:
			typedef unsigned __int64 uint64_type;

			typedef tls_entry tls_entry_type;
			typedef std::shared_ptr<tls_entry_type> tls_entry_ptr_type;

			typedef std::unordered_map<uint64_type, tls_entry_ptr_type> map_type;

			template <typename target_type>
			target_type *get(uint64_type key){
				auto entry = map_.find(key);
				return ((entry == map_.end()) ? nullptr : dynamic_cast<target_type *>(entry->second.get()));
			}

			template <typename target_type, typename... args_types>
			target_type *add(uint64_type key, args_types &&... args){
				return dynamic_cast<target_type *>((map_[key] = std::make_shared<target_type>(std::forward<args_types>(args)...)).get());
			}

			void remove(uint64_type key);

		private:
			static thread_local map_type map_;
		};
	}
}

#endif /* !OOSL_TLS_H */
