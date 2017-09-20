#pragma once

#ifndef OOSL_OUTPUT_STRING_WRITER_H
#define OOSL_OUTPUT_STRING_WRITER_H

#include "output_batch_writer.h"

namespace oosl{
	namespace common{
		template <class string_type, class wide_string_type>
		class output_string_writer : public output_batch_writer{
		public:
			typedef string_type string_type;
			typedef wide_string_type wide_string_type;

			output_string_writer(string_type &string, wide_string_type &wide_string)
				: string_(string), wide_string_(wide_string){}

			virtual ~output_string_writer() = default;

			virtual void write(const char *value, write_option_type options = write_option_type::nil) override{
				if (OOSL_IS(options, write_option_type::char_value)){
					wide_string_ += "'";
					wide_string_ += value;
					wide_string_ += "'";
				}
				else if (OOSL_IS(options, write_option_type::string_value)){
					wide_string_ += "\"";
					wide_string_ += value;
					wide_string_ += "\"";
				}
				else//No quotes
					wide_string_ += value;
			}

			virtual void write(const wchar_t *value, write_option_type options = write_option_type::nil) override{
				if (OOSL_IS(options, write_option_type::char_value)){
					wide_string_ += L"L'";
					wide_string_ += value;
					wide_string_ += L"'";
				}
				else if (OOSL_IS(options, write_option_type::string_value)){
					wide_string_ += L"L\"";
					wide_string_ += value;
					wide_string_ += L"\"";
				}
				else//No quotes
					wide_string_ += value;
			}

		protected:
			string_type &string_;
			wide_string_type &wide_string_;
		};
	}
}

#endif /* !OOSL_OUTPUT_STRING_WRITER_H */
