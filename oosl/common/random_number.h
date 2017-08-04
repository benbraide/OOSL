#pragma once

#ifndef OOSL_RANDOM_NUMBER_H
#define OOSL_RANDOM_NUMBER_H

#include <random>
#include <memory>
#include <climits>

namespace oosl{
	namespace common{
		template <class value_type, class distribution_type = std::uniform_int_distribution<value_type>, class engine_type = std::mt19937, class seeder_type = std::random_device>
		class basic_random_number{
		public:
			typedef value_type value_type;
			typedef engine_type engine_type;
			typedef distribution_type distribution_type;
			typedef seeder_type seeder_type;

			typedef std::pair<value_type, value_type> range_type;

			explicit basic_random_number(typename engine_type::result_type seed = (seeder_type())())
				: engine_(seed){}

			template <typename target_type = value_type>
			target_type operator ()(value_type from, value_type to){
				return generate<target_type>(from, to);
			}

			template <typename target_type = value_type>
			target_type operator ()(value_type from_zero_to){
				return generate<target_type>(from_zero_to);
			}

			template <typename target_type = value_type>
			target_type operator ()(const range_type &range){
				return generate<target_type>(range);
			}

			template <typename target_type = value_type>
			target_type generate(value_type from, value_type to){
				return static_cast<target_type>(distribution_type(from, to)(engine_));
			}

			template <typename target_type = value_type>
			target_type generate(value_type from_zero_to){
				return static_cast<target_type>(distribution_type(static_cast<value_type>(0), from_zero_to)(engine_));
			}

			template <typename target_type = value_type>
			target_type generate(const range_type &range){
				return static_cast<target_type>(distribution_type(range.first, range.second)(engine_));
			}

			template <typename target_type = value_type>
			target_type generate_unordered(const range_type &range){
				if (range.first < range.second)
					return static_cast<target_type>(distribution_type(range.first, range.second)(engine_));

				return static_cast<target_type>(distribution_type(range.second, range.first)(engine_));
			}

		private:
			engine_type engine_;
		};

		template <typename value_type>
		using basic_random_float = basic_random_number<value_type, std::uniform_real_distribution<value_type> >;

		typedef basic_random_number<__int8> random_int8;
		typedef basic_random_number<__int16> random_int16;
		typedef basic_random_number<__int32> random_int32;
		typedef basic_random_number<__int64> random_int64;

		typedef basic_random_number<unsigned __int8> random_uint8;
		typedef basic_random_number<unsigned __int16> random_uint16;
		typedef basic_random_number<unsigned __int32> random_uint32;
		typedef basic_random_number<unsigned __int64> random_uint64;

		typedef basic_random_number<short> random_short;
		typedef basic_random_number<int> random_int;
		typedef basic_random_number<long> random_long;
		typedef basic_random_number<long long> random_long_long;

		typedef basic_random_number<unsigned short> random_ushort;
		typedef basic_random_number<unsigned int> random_uint;
		typedef basic_random_number<unsigned long> random_ulong;
		typedef basic_random_number<unsigned long long> random_ulong_long;

		typedef basic_random_float<float> random_float;
		typedef basic_random_float<double> random_double;
		typedef basic_random_float<long double> random_long_double;
	}
}

#endif /* !OOSL_RANDOM_NUMBER_H */
