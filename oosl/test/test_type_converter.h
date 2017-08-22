#pragma once

#ifndef OOSL_TEST_TYPE_CONVERTER_H
#define OOSL_TEST_TYPE_CONVERTER_H

#include "test_include.h"
#include "../type/type_converter.h"

DOCTEST_TEST_CASE("type converter test"){
	__int64 int_value = 108;
	float float_value = 4.5f;

	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), __int8>(reinterpret_cast<char *>(&int_value)) == static_cast<__int8>(108));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), __int16>(reinterpret_cast<char *>(&int_value)) == static_cast<__int16>(108));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), __int32>(reinterpret_cast<char *>(&int_value)) == static_cast<__int32>(108));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), __int64>(reinterpret_cast<char *>(&int_value)) == static_cast<__int64>(108));

	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), float>(reinterpret_cast<char *>(&int_value)) == static_cast<float>(108));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), double>(reinterpret_cast<char *>(&int_value)) == static_cast<double>(108));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(int_value), long double>(reinterpret_cast<char *>(&int_value)) == static_cast<long double>(108));

	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), __int8>(reinterpret_cast<char *>(&float_value)) == static_cast<__int8>(4.5f));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), __int16>(reinterpret_cast<char *>(&float_value)) == static_cast<__int16>(4.5f));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), __int32>(reinterpret_cast<char *>(&float_value)) == static_cast<__int32>(4.5f));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), __int64>(reinterpret_cast<char *>(&float_value)) == static_cast<__int64>(4.5f));

	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), float>(reinterpret_cast<char *>(&float_value)) == static_cast<float>(4.5f));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), double>(reinterpret_cast<char *>(&float_value)) == static_cast<double>(4.5f));
	DOCTEST_CHECK(oosl::type::converter::convert<decltype(float_value), long double>(reinterpret_cast<char *>(&float_value)) == static_cast<long double>(4.5f));
}

#endif /* !OOSL_TEST_TYPE_CONVERTER_H */
