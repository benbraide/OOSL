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

	DOCTEST_SUBCASE("allocation test"){
		auto first_allocation = manager.allocate(8u);
		DOCTEST_REQUIRE(first_allocation != nullptr);

		auto second_allocation = manager.allocate(4u);
		DOCTEST_REQUIRE(second_allocation != nullptr);

		auto third_allocation = manager.allocate(0u);
		DOCTEST_REQUIRE(third_allocation != nullptr);

		auto fourth_allocation = manager.allocate<float>();
		DOCTEST_REQUIRE(fourth_allocation != nullptr);

		auto reserved_address = manager.reserve(32u);
		DOCTEST_REQUIRE(reserved_address > fourth_allocation->address);

		auto constant_allocation = manager.allocate_scalar("oosl");
		DOCTEST_REQUIRE(constant_allocation != nullptr);

		auto fifth_allocation = manager.allocate(32u, reserved_address);
		DOCTEST_REQUIRE(fifth_allocation != nullptr);

		DOCTEST_CHECK_THROWS(manager.allocate(2u, protected_region));
		DOCTEST_CHECK_THROWS(manager.allocate(2u, third_allocation->address));

		DOCTEST_SUBCASE("actual allocation test"){
			DOCTEST_CHECK(first_allocation->address == (protected_region + 1));
			DOCTEST_CHECK(first_allocation->size == 8u);
			DOCTEST_CHECK(first_allocation->size == first_allocation->actual_size);
			DOCTEST_CHECK(first_allocation->attributes == oosl::memory::manager::attribute_type::nil);

			DOCTEST_CHECK(second_allocation->address > first_allocation->address);
			DOCTEST_CHECK(second_allocation->size == 4u);
			DOCTEST_CHECK(second_allocation->size == second_allocation->actual_size);
			DOCTEST_CHECK(second_allocation->attributes == oosl::memory::manager::attribute_type::nil);

			DOCTEST_CHECK(third_allocation->address > second_allocation->address);
			DOCTEST_CHECK(third_allocation->size == 0u);
			DOCTEST_CHECK(third_allocation->size != third_allocation->actual_size);
			DOCTEST_CHECK(third_allocation->attributes == oosl::memory::manager::attribute_type::nil);

			DOCTEST_CHECK(fourth_allocation->address > third_allocation->address);
			DOCTEST_CHECK(fourth_allocation->size == sizeof(float));
			DOCTEST_CHECK(fourth_allocation->size == fourth_allocation->actual_size);
			DOCTEST_CHECK(fourth_allocation->attributes != oosl::memory::manager::attribute_type::nil);

			DOCTEST_CHECK(fifth_allocation->address == reserved_address);
			DOCTEST_CHECK(fifth_allocation->size == 32u);
			DOCTEST_CHECK(fifth_allocation->size == fifth_allocation->actual_size);
			DOCTEST_CHECK(fifth_allocation->attributes == oosl::memory::manager::attribute_type::nil);

			DOCTEST_CHECK(constant_allocation->address > fifth_allocation->address);
			DOCTEST_CHECK(constant_allocation->size == 5u);
			DOCTEST_CHECK(constant_allocation->size == constant_allocation->actual_size);
			DOCTEST_CHECK(constant_allocation->attributes != oosl::memory::manager::attribute_type::nil);
		}

		DOCTEST_SUBCASE("deallocation test"){
			auto address = first_allocation->address;
			DOCTEST_CHECK(manager.find_block(first_allocation->address) != nullptr);
			manager.deallocate(first_allocation->address);
			DOCTEST_CHECK(manager.find_block(first_allocation->address) == nullptr);
			DOCTEST_CHECK_THROWS(manager.deallocate(address));
		}

		DOCTEST_SUBCASE("reallocation test"){
			auto size = first_allocation->size;
			auto entry = manager.reallocate(first_allocation->address, (first_allocation->size - 4u));//Shrink

			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address == first_allocation->address);
			DOCTEST_CHECK(entry->size == (size - 4u));
			DOCTEST_CHECK(entry->size != entry->actual_size);

			size = fifth_allocation->size;
			entry = manager.reallocate(fifth_allocation->address, (fifth_allocation->size + 4u));//Expand

			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address == fifth_allocation->address);
			DOCTEST_CHECK(entry->size == (size + 4u));
			DOCTEST_CHECK(entry->size == entry->actual_size);

			size = first_allocation->size;
			entry = manager.reallocate(first_allocation->address, (first_allocation->size + 4u));//Expand

			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address >= first_allocation->address);
			DOCTEST_CHECK(entry->size == (size + 4u));
			DOCTEST_CHECK(entry->size == entry->actual_size);

			auto address = first_allocation->address;
			size = first_allocation->size;
			entry = manager.reallocate(first_allocation->address, (first_allocation->size + 4u));//Expand

			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address != address);
			DOCTEST_CHECK(entry->size == (size + 4u));
			DOCTEST_CHECK(entry->size == entry->actual_size);
		}

		DOCTEST_SUBCASE("fill and copy test"){
			manager.fill(first_allocation->address, '\0', first_allocation->size);
			DOCTEST_CHECK(*reinterpret_cast<long long *>(first_allocation->ptr) == 0ll);

			manager.copy(second_allocation->address, first_allocation->address, second_allocation->size);
			DOCTEST_CHECK(*reinterpret_cast<int *>(second_allocation->ptr) == 0);
		}

		DOCTEST_SUBCASE("write and read test"){
			auto value = 72ll;
			manager.write(first_allocation->address, reinterpret_cast<const char *>(&value), first_allocation->size);
			DOCTEST_CHECK(*reinterpret_cast<long long *>(first_allocation->ptr) == 72ll);

			long long read_value;
			manager.read(first_allocation->address, reinterpret_cast<char *>(&read_value), first_allocation->size);

			DOCTEST_CHECK(read_value == 72ll);
			DOCTEST_CHECK(manager.read<long long>(first_allocation->address) == 72ll);
			DOCTEST_CHECK(manager.read_numeric<int>(first_allocation->address) == 72);
			DOCTEST_CHECK(manager.read_numeric<float>(first_allocation->address) == 72.0f);

			manager.write_numeric(fourth_allocation->address, 4.5f);
			DOCTEST_CHECK(manager.read<float>(fourth_allocation->address) == doctest::Approx(4.5f));
			DOCTEST_CHECK(manager.read_numeric<float>(fourth_allocation->address) == doctest::Approx(4.5f));

			DOCTEST_CHECK_THROWS(manager.write(constant_allocation->address, "pptm", 5u));
		}

		DOCTEST_SUBCASE("search test"){
			auto entry = manager.find_block(first_allocation->address);

			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address == first_allocation->address);

			DOCTEST_CHECK(manager.find_block(first_allocation->address + 2) == nullptr);

			entry = manager.find_enclosing_block(first_allocation->address + 2);
			DOCTEST_REQUIRE(entry != nullptr);
			DOCTEST_CHECK(entry->address == first_allocation->address);
		}
	}
}

#endif /* !OOSL_TEST_MEMORY_MANAGER_H */
