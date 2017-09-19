#pragma once

#ifndef OOSL_OUTPUT_STREAM_WRITER_H
#define OOSL_OUTPUT_STREAM_WRITER_H

#include "output_batch_writer.h"

namespace oosl{
	namespace common{
		template <class stream_type, class wide_stream_type>
		class output_stream_writer : public output_batch_writer{
		public:
			typedef stream_type stream_type;

			output_stream_writer(stream_type &stream, wide_stream_type &wide_stream)
				: stream_(stream), wide_stream_(wide_stream){}

			virtual ~output_stream_writer() = default;

			virtual void write(const char *value, write_option_type options = write_option_type::nil) override{
				if (OOSL_IS(options, write_option_type::char_value))
					stream_ << "'" << value << "'";
				else if (OOSL_IS(options, write_option_type::string_value))
					stream_ << "\"" << value << "\"";
				else//No quotes
					stream_ << value;
			}

			virtual void write(const wchar_t *value, write_option_type options = write_option_type::nil) override{
				if (OOSL_IS(options, write_option_type::char_value))
					wide_stream_ << L"L'" << value << L"'";
				else if (OOSL_IS(options, write_option_type::string_value))
					wide_stream_ << L"L\"" << value << L"\"";
				else//No quotes
					wide_stream_ << value;
			}

		protected:
			stream_type &stream_;
			wide_stream_type &wide_stream_;
		};
	}
}

#endif /* !OOSL_OUTPUT_STREAM_WRITER_H */
