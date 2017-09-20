#pragma once

#ifndef OOSL_TEST_DRIVERS_H
#define OOSL_TEST_DRIVERS_H

#include "test_include.h"

#include "../common/output_string_writer.h"
#include "../common/controller_impl.h"

OOSL_ADD_TEST(drivers_test, "drivers test");

DOCTEST_TEST_CASE("drivers test"){
	oosl::common::controller_impl controller_impl_instance;
	oosl::storage::temporary temp;

	std::string output_buffer;
	std::wstring wide_output_buffer;
	oosl::common::output_string_writer<std::string, std::wstring> output_writer(output_buffer, wide_output_buffer);

	controller_impl_instance.runtime_info().temporary_storage = &temp;
	controller_impl_instance.output_writer(oosl::common::controller::output_writer_key_type::nil, output_writer);

	DOCTEST_SUBCASE("boolean driver test"){
		auto false_value = controller_impl_instance.find_static_value(oosl::common::controller::static_value_type::false_);
		auto true_value = controller_impl_instance.find_static_value(oosl::common::controller::static_value_type::true_);

		false_value->type->driver()->echo(*false_value);
		DOCTEST_CHECK(output_buffer == "false");

		output_buffer.clear();
		true_value->type->driver()->echo(*true_value);
		DOCTEST_CHECK(output_buffer == "true");
	}

	DOCTEST_SUBCASE("numeric driver test"){

	}
}

#endif /* !OOSL_TEST_DRIVERS_H */
