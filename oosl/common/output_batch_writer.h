#pragma once

#ifndef OOSL_OUTPUT_BATCH_WRITER_H
#define OOSL_OUTPUT_BATCH_WRITER_H

#include <mutex>

#include "output_writer.h"

namespace oosl{
	namespace common{
		class output_batch_writer : public output_writer{
		public:
			typedef std::mutex lock_type;

			virtual ~output_batch_writer();

			virtual void begin() override;

			virtual void end() override;

		protected:
			lock_type lock_;
		};
	}
}

#endif /* !OOSL_OUTPUT_BATCH_WRITER_H */
