#pragma once

#ifndef OOSL_TYPE_CONVERTER_H
#define OOSL_TYPE_CONVERTER_H

#include <cstdlib>

#include "type_id.h"
#include "../common/error_codes.h"

namespace oosl{
	namespace type{
		class converter{
		public:
			typedef id id_type;

			template <class source_type, class destination_type>
			static destination_type convert(const char *buffer){
				auto value = source_type();
				memcpy_s(&value, sizeof(source_type), buffer, sizeof(source_type));
				return static_cast<destination_type>(value);
			}

			template <class source_type, class destination_type>
			static void convert(source_type value, char *destination){
				auto compatible_value = static_cast<destination_type>(value);
				memcpy_s(destination, sizeof(destination_type), &compatible_value, sizeof(destination_type));
			}

			template <class source_type, class destination_type>
			static void convert(const char *source, char *destination){
				auto compatible_value = convert<source_type, destination_type>(source);
				memcpy_s(destination, sizeof(destination_type), &compatible_value, sizeof(destination_type));
			}

			template <class source_type>
			static void convert(id_type to, const char *source, char *destination){
				switch (to){
				case id_type::char_:
					convert<source_type, char>(source, destination);
					break;
				case id_type::wchar_:
					convert<source_type, wchar_t>(source, destination);
					break;
				case id_type::int8_:
					convert<source_type, __int8>(source, destination);
					break;
				case id_type::uint8_:
					convert<source_type, unsigned __int8>(source, destination);
					break;
				case id_type::int16_:
					convert<source_type, __int16>(source, destination);
					break;
				case id_type::uint16_:
					convert<source_type, unsigned __int16>(source, destination);
					break;
				case id_type::int32_:
					convert<source_type, __int32>(source, destination);
					break;
				case id_type::uint32_:
					convert<source_type, unsigned __int32>(source, destination);
					break;
				case id_type::int64_:
					convert<source_type, __int64>(source, destination);
					break;
				case id_type::uint64_:
					convert<source_type, unsigned __int64>(source, destination);
					break;
				case id_type::float_:
					convert<source_type, float>(source, destination);
					break;
				case id_type::double_:
					convert<source_type, double>(source, destination);
					break;
				case id_type::ldouble:
					convert<source_type, long double>(source, destination);
					break;
				default:
					throw common::error_codes::incompatible_types;
					break;
				}
			}

			static void convert(id_type from, id_type to, const char *source, char *destination){
				switch (from){
				case id_type::char_:
					convert<char>(to, source, destination);
					break;
				case id_type::wchar_:
					convert<wchar_t>(to, source, destination);
					break;
				case id_type::int8_:
					convert<__int8>(to, source, destination);
					break;
				case id_type::uint8_:
					convert<unsigned __int8>(to, source, destination);
					break;
				case id_type::int16_:
					convert<__int16>(to, source, destination);
					break;
				case id_type::uint16_:
					convert<unsigned __int16>(to, source, destination);
					break;
				case id_type::int32_:
					convert<__int32>(to, source, destination);
					break;
				case id_type::uint32_:
					convert<unsigned __int32>(to, source, destination);
					break;
				case id_type::int64_:
					convert<__int64>(to, source, destination);
					break;
				case id_type::uint64_:
					convert<unsigned __int64>(to, source, destination);
					break;
				case id_type::float_:
					convert<float>(to, source, destination);
					break;
				case id_type::double_:
					convert<double>(to, source, destination);
					break;
				case id_type::ldouble:
					convert<long double>(to, source, destination);
					break;
				default:
					throw common::error_codes::incompatible_types;
					break;
				}
			}
		};
	}
}

#endif /* !OOSL_TYPE_CONVERTER_H */
