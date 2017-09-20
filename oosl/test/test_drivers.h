#pragma once

#ifndef OOSL_TEST_DRIVERS_H
#define OOSL_TEST_DRIVERS_H

#include "test_include.h"
#include "../common/controller_impl.h"

DOCTEST_TEST_CASE("drivers test"){
	oosl::common::controller_impl controller_impl_instance;
	oosl::storage::temporary temp;

	controller_impl_instance.runtime_info().temporary_storage = &temp;
	DOCTEST_SUBCASE("numeric driver test"){

	}
}

#endif /* !OOSL_TEST_DRIVERS_H */
