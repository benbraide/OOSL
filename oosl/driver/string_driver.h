#pragma once

#ifndef OOSL_STRING_DRIVER_H
#define OOSL_STRING_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class string_driver : public object{
		public:
			using object::echo;
			using object::value;

			virtual ~string_driver();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand) override;

			template <typename target_type>
			void echo_(entry_type &entry, output_writer_type &writer){
				target_type target;
				string_value_(entry, target);
				writer.write(target.c_str(), write_option_type::string_value);
			}

			template <typename target_type>
			void string_value_(entry_type &entry, target_type &target){
				typedef typename target_type::traits_type::char_type char_type;

				auto string_address = oosl::common::controller::active->memory().read<uint64_type>(entry.address);
				auto string_block = oosl::common::controller::active->memory().find_block(string_address);

				if (string_block == nullptr){//Copy value
					if (string_block->size > 1u)
						target.assign((char_type *)string_block->ptr, (string_block->size - 1));
				}
				else//Error
					throw error_type::invalid_address;
			}

			template <typename from_type>
			void value_from_(entry_type &entry, type_id_type to, char *destination){
				switch (to){
				case type_id_type::string_:
				case type_id_type::wstring_:
					return string_value_(entry, *reinterpret_cast<from_type *>(destination));
				default:
					break;
				}

				throw error_type::not_implemented;
			}

			template <typename target_type>
			entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, entry_type &operand, type_id_type string_id){
				switch (operator_id){
				case operator_id_type::plus://Return string
					return concatenate_string_<target_type>(entry, operand, string_id);
				default:
					break;
				}

				return evaluate_boolean_<target_type>(entry, operator_id, operand);
			}
		};
	}
}

#endif /* !OOSL_STRING_DRIVER_H */
