#pragma once

#ifndef OOSL_STRING_CONVERTER_H
#define OOSL_STRING_CONVERTER_H

#include <string>

namespace oosl{
	namespace common{
		template <class target_type>
		struct string_converter;

		template <>
		struct string_converter<std::string>{
			static std::string convert(const std::string &value){
				return value;
			}
		};

		template <>
		struct string_converter<short>{
			static short convert(const std::string &value){
				return static_cast<short>(std::stoi(value));
			}
		};

		template <>
		struct string_converter<unsigned short>{
			static unsigned short convert(const std::string &value){
				return static_cast<unsigned short>(std::stoi(value));
			}
		};

		template <>
		struct string_converter<int>{
			static int convert(const std::string &value){
				return std::stoi(value);
			}
		};

		template <>
		struct string_converter<unsigned int>{
			static unsigned int convert(const std::string &value){
				return static_cast<unsigned int>(std::stoi(value));
			}
		};

		template <>
		struct string_converter<long>{
			static long convert(const std::string &value){
				return std::stol(value);
			}
		};

		template <>
		struct string_converter<unsigned long>{
			static unsigned long convert(const std::string &value){
				return std::stoul(value);
			}
		};

		template <>
		struct string_converter<long long>{
			static long long convert(const std::string &value){
				return std::stoll(value);
			}
		};

		template <>
		struct string_converter<unsigned long long>{
			static unsigned long long convert(const std::string &value){
				return std::stoull(value);
			}
		};

		template <>
		struct string_converter<float>{
			static float convert(const std::string &value){
				return std::stof(value);
			}
		};

		template <>
		struct string_converter<double>{
			static double convert(const std::string &value){
				return std::stod(value);
			}
		};

		template <>
		struct string_converter<long double>{
			static long double convert(const std::string &value){
				return std::stold(value);
			}
		};
	}
}

#endif /* !OOSL_STRING_CONVERTER_H */
