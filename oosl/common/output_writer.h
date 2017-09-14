#pragma once

#ifndef OOSL_OUTPUT_WRITER_H
#define OOSL_OUTPUT_WRITER_H

namespace oosl{
	namespace common{
		class output_writer{
		public:
			virtual ~output_writer() = default;

			virtual void write(const char *value) = 0;

			virtual void write(const wchar_t *value) = 0;

			virtual void error(const char *value) = 0;

			virtual void error(const wchar_t *value) = 0;
		};
	}
}

#endif /* !OOSL_OUTPUT_WRITER_H */
