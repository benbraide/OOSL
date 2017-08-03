#include "lock_once.h"

thread_local bool oosl::common::lock_once_info::lock_is_acquired = false;
