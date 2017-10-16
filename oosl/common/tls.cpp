#include "tls.h"

oosl::common::tls_entry::~tls_entry() = default;

void oosl::common::tls::remove(uint64_type key){
	map_.erase(key);
}

thread_local oosl::common::tls::map_type oosl::common::tls::map_;
