#pragma once

#ifndef OOSL_TEST_MEMORY_MANAGER_H
#define OOSL_TEST_MEMORY_MANAGER_H

#include "test_include.h"
#include "../memory/memory_manager.h"

DOCTEST_TEST_CASE("memory manager test"){
	static const oosl::memory::manager::uint64_type protected_region = 1024ull;
	oosl::memory::manager manager(protected_region);

	DOCTEST_SUBCASE("protected region test"){
		DOCTEST_CHECK(manager.is_protected(0ull));
		DOCTEST_CHECK(manager.is_protected(protected_region));
		DOCTEST_CHECK(!manager.is_protected(protected_region + 1));

		DOCTEST_CHECK_THROWS(manager.find_block(0ull));
		DOCTEST_CHECK_THROWS(manager.find_block(protected_region));
		DOCTEST_CHECK_NOTHROW(manager.find_block(protected_region + 1));

		manager.enter_protected_mode();
		DOCTEST_CHECK_NOTHROW(manager.find_block(0ull));
		DOCTEST_CHECK_NOTHROW(manager.find_block(protected_region));
		DOCTEST_CHECK_NOTHROW(manager.find_block(protected_region + 1));

		manager.leave_protected_mode();
		DOCTEST_CHECK_THROWS(manager.find_block(0ull));
		DOCTEST_CHECK_THROWS(manager.find_block(protected_region));
		DOCTEST_CHECK_NOTHROW(manager.find_block(protected_region + 1));
	}
}

#endif /* !OOSL_TEST_MEMORY_MANAGER_H */
