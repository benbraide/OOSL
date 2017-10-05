#pragma once

#ifndef OOSL_OUTPUT_WRITER_H
#define OOSL_OUTPUT_WRITER_H

#include <string>

#include "preprocessor.h"

namespace oosl{
	namespace common{
		class output_writer{
		public:
			enum class write_option_type : unsigned int{
				nil					= (0 << 0x0000),
				char_value			= (1 << 0x0000),
				string_value		= (1 << 0x0001),
			};

			virtual ~output_writer() = default;

			virtual void begin() = 0;

			virtual void end() = 0;

			virtual void write(const std::string &value, write_option_type options = write_option_type::nil){
				write(value.c_str(), options);
			}

			virtual void write(const char *value, write_option_type options = write_option_type::nil) = 0;

			virtual void write(const std::wstring &value, write_option_type options = write_option_type::nil){
				write(value.c_str(), options);
			}

			virtual void write(const wchar_t *value, write_option_type options = write_option_type::nil) = 0;
		};

		OOSL_MAKE_OPERATORS(output_writer::write_option_type);
	}
}

#endif /* !OOSL_OUTPUT_WRITER_H */
