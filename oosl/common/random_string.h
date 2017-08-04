#pragma once

#ifndef OOSL_RANDOM_STRING_H
#define OOSL_RANDOM_STRING_H

#include <string>
#include <vector>

#include "preprocessor.h"
#include "random_number.h"

namespace oosl{
	namespace common{
		enum class random_string_char_set : unsigned int{
			nil					= (0 << 0x0000),
			digits				= (1 << 0x0000),
			alpha				= (1 << 0x0001),
			uppercase			= (1 << 0x0002),
			lowercase			= (1 << 0x0003),
			full_range			= (1 << 0x0004),
		};

		template <class char_type, class engine_type = std::mt19937, class distribution_type = std::uniform_int_distribution<std::size_t>, class seeder_type = std::random_device>
		class basic_random_string{
		public:
			typedef random_string_char_set char_set_type;

			typedef engine_type engine_type;
			typedef distribution_type distribution_type;
			typedef seeder_type seeder_type;

			typedef std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type> > string_type;
			typedef typename string_type::size_type size_type;

			typedef std::pair<char_type, char_type> range_type;
			typedef std::vector<range_type> domain_type;

			typedef basic_random_number<size_type, distribution_type, engine_type, seeder_type> random_type;

			explicit basic_random_string(typename engine_type::result_type seed = (seeder_type())())
				: engine_(seed){}

			string_type operator ()(size_type length, const domain_type &domain){
				return generate(length, domain);
			}

			string_type operator ()(size_type length, char_set_type set){
				return generate(length, set);
			}

			string_type operator ()(const std::pair<size_type, size_type> &range, const domain_type &domain){
				return generate(range, domain);
			}

			string_type operator ()(const std::pair<size_type, size_type> &range, char_set_type set){
				return generate(range, set);
			}

			string_type operator ()(size_type length){
				return generate(length);
			}

			string_type operator ()(const std::pair<size_type, size_type> &range){
				return generate(range);
			}

			string_type generate(size_type length, const domain_type &domain){
				string_type generated;
				if (!domain.empty()){
					while (generated.size() < length){
						auto index = engine_.generate(domain.size() - 1);
						auto char_value = engine_.generate<char_type>(domain[index].first, domain[index].second);
						generated.append(1, char_value);
					}
				}

				return generated;
			}

			string_type generate(size_type length, char_set_type set){
				domain_type domain;
				get_domain(set, domain);
				return generate(length, domain);
			}

			string_type generate(const std::pair<size_type, size_type> &range, const domain_type &domain){
				return generate(engine_.generate(range.first, range.second), domain);
			}

			string_type generate(const std::pair<size_type, size_type> &range, char_set_type set){
				return generate(engine_.generate(range.first, range.second), set);
			}

			string_type generate(size_type length){
				return generate(length, char_set_type::nil);
			}

			string_type generate(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::nil);
			}

			string_type generate_alpha(size_type length){
				return generate(length, char_set_type::alpha);
			}

			string_type generate_alpha(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::alpha);
			}

			string_type generate_lowercase_alpha(size_type length){
				return generate(length, char_set_type::alpha | char_set_type::lowercase);
			}

			string_type generate_lowercase_alpha(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::alpha | char_set_type::lowercase);
			}

			string_type generate_uppercase_alpha(size_type length){
				return generate(length, char_set_type::alpha | char_set_type::uppercase);
			}

			string_type generate_uppercase_alpha(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::alpha | char_set_type::uppercase);
			}

			string_type generate_digit(size_type length){
				return generate(length, char_set_type::digits);
			}

			string_type generate_digit(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::digits);
			}

			string_type generate_alnum(size_type length){
				return generate(length, char_set_type::digits | char_set_type::alpha);
			}

			string_type generate_alnum(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::digits | char_set_type::alpha);
			}

			string_type generate_lowercase_alnum(size_type length){
				return generate(length, char_set_type::digits | char_set_type::alpha | char_set_type::lowercase);
			}

			string_type generate_lowercase_alnum(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::digits | char_set_type::alpha | char_set_type::lowercase);
			}

			string_type generate_uppercase_alnum(size_type length){
				return generate(length, char_set_type::digits | char_set_type::alpha | char_set_type::uppercase);
			}

			string_type generate_uppercase_alnum(const std::pair<size_type, size_type> &range){
				return generate(range, char_set_type::digits | char_set_type::alpha | char_set_type::uppercase);
			}

			void get_domain(char_set_type set, domain_type &domain) const{
				if (!OOSL_IS(set, char_set_type::full_range)){
					if (set != char_set_type::nil){
						if (OOSL_IS(set, char_set_type::digits))
							domain.push_back(std::make_pair<char_type, char_type>(0x0030, 0x0039));

						if (OOSL_IS(set, char_set_type::alpha)){
							if (OOSL_IS_ANY(set, char_set_type::uppercase | char_set_type::lowercase)){
								if (OOSL_IS(set, char_set_type::uppercase))
									domain.push_back(std::make_pair<char_type, char_type>(0x0041, 0x005A));

								if (OOSL_IS(set, char_set_type::lowercase))
									domain.push_back(std::make_pair<char_type, char_type>(0x0061, 0x007A));
							}
							else{//Both
								domain.push_back(std::make_pair<char_type, char_type>(0x0041, 0x005A));
								domain.push_back(std::make_pair<char_type, char_type>(0x0061, 0x007A));
							}
						}
					}
					else{//Default
						domain.push_back(std::make_pair<char_type, char_type>(0x0030, 0x0039));
						domain.push_back(std::make_pair<char_type, char_type>(0x0041, 0x005A));
						domain.push_back(std::make_pair<char_type, char_type>(0x0061, 0x007A));
					}
				}
				else//Full character range
					domain.push_back(std::make_pair<char_type, char_type>(0, std::numeric_limits<char_type>::max()));
			}

		private:
			random_type engine_;
		};

		typedef basic_random_string<char> random_string;
		typedef basic_random_string<wchar_t> random_wstring;
	}
}

#endif /* !OOSL_RANDOM_STRING_H */
