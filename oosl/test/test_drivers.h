#pragma once

#ifndef OOSL_TEST_DRIVERS_H
#define OOSL_TEST_DRIVERS_H

#include "test_include.h"

#include "../common/output_string_writer.h"
#include "../common/controller_impl.h"

OOSL_ADD_TEST(drivers_test, "drivers test");

DOCTEST_TEST_CASE("drivers test"){
	typedef oosl::type::id type_id_type;
	typedef oosl::driver::object::uint8_type uint8_type;
	typedef oosl::common::operator_id operator_id;
	typedef oosl::common::controller controller_type;

	oosl::common::controller::active = nullptr;
	oosl::common::controller_impl controller_impl_instance;
	oosl::storage::temporary temp;

	std::string output_buffer;
	std::wstring wide_output_buffer;
	oosl::common::output_string_writer<std::string, std::wstring> output_writer(output_buffer, wide_output_buffer);

	controller_impl_instance.runtime_info().temporary_storage = &temp;
	controller_impl_instance.output_writer(controller_type::output_writer_key_type::nil, output_writer);

	oosl::common::operator_info unary_info{ operator_id::relational_not };
	oosl::common::operator_info binary_info{ operator_id::equality };

	DOCTEST_SUBCASE("boolean driver test"){
		auto false_value = controller_impl_instance.find_static_value(controller_type::static_value_type::false_);
		auto true_value = controller_impl_instance.find_static_value(controller_type::static_value_type::true_);

		false_value->type->driver()->echo(*false_value);
		DOCTEST_CHECK(output_buffer == "false");

		output_buffer.clear();
		true_value->type->driver()->echo(*true_value);

		DOCTEST_CHECK(output_buffer == "true");
		output_buffer.clear();

		auto result = false_value->type->driver()->evaluate(*false_value, unary_info);
		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == "true");

		output_buffer.clear();
		result = true_value->type->driver()->evaluate(*true_value, unary_info);

		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == "false");

		output_buffer.clear();
		result = false_value->type->driver()->evaluate(*false_value, binary_info, *true_value);

		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == "false");

		binary_info.id = operator_id::inverse_equality;
		output_buffer.clear();
		result = false_value->type->driver()->evaluate(*false_value, binary_info, *true_value);

		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == "true");
		DOCTEST_CHECK(result->type->driver()->value<oosl::type::bool_type>(*result) == oosl::type::bool_type::true_);
	}

	DOCTEST_SUBCASE("byte driver test"){
		auto value = controller_impl_instance.temporary_storage().add_scalar(static_cast<uint8_type>(72), controller_impl_instance.find_type(type_id_type::byte_));
		auto value2 = controller_impl_instance.temporary_storage().add_scalar(static_cast<uint8_type>(9), controller_impl_instance.find_type(type_id_type::byte_));

		value->type->driver()->echo(*value);
		DOCTEST_CHECK(output_buffer == controller_type::convert_base(value->type->driver()->value<uint8_type>(*value), 16, (sizeof(uint8_type) << 1)));

		output_buffer.clear();
		unary_info.id = operator_id::bitwise_inverse;

		auto result = value->type->driver()->evaluate(*value, unary_info);
		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == controller_type::convert_base(static_cast<uint8_type>(~value->type->driver()->value<uint8_type>(*value)), 16, (sizeof(uint8_type) << 1)));

		output_buffer.clear();
		binary_info.id = operator_id::bitwise_and;

		result = value->type->driver()->evaluate(*value, binary_info, *value2);
		result->type->driver()->echo(*result);
		DOCTEST_CHECK(output_buffer == controller_type::convert_base(static_cast<uint8_type>(value->type->driver()->value<uint8_type>(*value) & 9), 16, (sizeof(uint8_type) << 1)));
	}
}

#endif /* !OOSL_TEST_DRIVERS_H */
