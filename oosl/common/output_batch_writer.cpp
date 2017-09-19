#include "output_batch_writer.h"

oosl::common::output_batch_writer::~output_batch_writer() = default;

void oosl::common::output_batch_writer::begin(){
	lock_.lock();
}

void oosl::common::output_batch_writer::end(){
	lock_.unlock();
}
