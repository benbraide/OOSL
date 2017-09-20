#pragma once

#ifndef OOSL_CONTROLLER_IMPL_H
#define OOSL_CONTROLLER_IMPL_H

#include <iostream>

#include "controller.h"
#include "structures.h"
#include "output_stream_writer.h"

#include "../driver/boolean_driver.h"
#include "../driver/byte_driver.h"
#include "../driver/char_driver.h"
#include "../driver/numeric_driver.h"
#include "../driver/string_driver.h"
#include "../driver/pointer_driver.h"
#include "../driver/dynamic_driver.h"

#include "../type/any_type.h"
#include "../type/numeric_type.h"
#include "../type/pointer_type.h"
#include "../type/static_array.h"
#include "../type/variant_type.h"
#include "../type/function_type.h"
#include "../type/modified_type.h"

#include "../storage/named_storage.h"

namespace oosl{
	namespace common{
		class controller_impl : public controller{
		public:
			typedef oosl::storage::named named_storage_type;
			typedef output_stream_writer<std::ostream, std::wostream> output_stream_writer_type;

			typedef std::unordered_map<type_id_type, type_object_ptr_type> type_object_ptr_list_type;
			typedef std::unordered_map<static_value_type, storage_entry_type *> static_value_list_type;
			typedef std::unordered_map<driver_type, std::shared_ptr<driver_object_type>> driver_object_list_type;

			controller_impl();

			virtual ~controller_impl();

			virtual bool exiting() override;

			virtual interpreter_info_type &interpreter_info() override;

			virtual runtime_info_type &runtime_info() override;

			virtual void on_exception(callback_type callback) override;

			virtual void on_exception_pop() override;

			virtual output_writer_type &output_writer() override;

			virtual output_writer_type &error_output_writer() override;

			virtual memory_manager_type &memory() override;

			virtual storage_object_type &global_storage() override;

			virtual temporary_storage_type &temporary_storage() override;

			virtual type_object_ptr_type find_type(type_id_type id) override;

			virtual storage_entry_type *find_static_value(static_value_type type) override;

			virtual driver_object_type *find_driver(driver_type type) override;

		protected:
			static thread_local runtime_info_type runtime_info_;

			memory_manager_type memory_manager_;
			temporary_storage_type internal_temporary_storage_;
			named_storage_type global_storage_;
			output_stream_writer_type output_stream_writer_;
			output_stream_writer_type error_output_stream_writer_;
			type_object_ptr_list_type type_list_;
			static_value_list_type static_value_list_;
			driver_object_list_type driver_list_;
		};
	}
}

#endif /* !OOSL_CONTROLLER_IMPL_H */
