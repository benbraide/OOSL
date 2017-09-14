#pragma once

#ifndef OOSL_CHAR_DRIVER_H
#define OOSL_CHAR_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class char_driver : public object{
		public:
			using object::echo;
			using object::value;

			virtual ~char_driver();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand) override;

			template <typename target_type>
			void echo_(entry_type &entry, output_writer_type &writer, type_id_type id){
				target_type target;
				value(entry, id, reinterpret_cast<char *>(&target));
				writer.write(target.c_str());
			}

			template <typename from_type>
			void value_from_(entry_type &entry, type_id_type to, char *destination){
				switch (to){
				case type_id_type::char_:
					return value_<from_type, char>(entry, to, destination);
				case type_id_type::wchar_:
					return value_<from_type, wchar_t>(entry, to, destination);
				case type_id_type::string_:
					return string_value_<from_type, std::string>(entry, destination, std::bool_constant<std::is_same<from_type, char>::value>());
				case type_id_type::wstring_:
					return string_value_<from_type, std::wstring>(entry, destination, std::bool_constant<std::is_same<from_type, wchar_t>::value>());
				default:
					break;
				}

				throw error_type::not_implemented;
			}

			template <typename from_type, typename to_type>
			void value_(entry_type &entry, type_id_type to, char *destination){
				auto value = static_cast<to_type>(oosl::common::controller::active->memory().read<from_type>(entry.address));
				memcpy(destination, &value, sizeof(to_type));
			}

			template <typename from_type, typename string_type>
			void string_value_(entry_type &entry, char *destination, std::true_type){
				*reinterpret_cast<string_type *>(destination) = string_type(1u, oosl::common::controller::active->memory().read<from_type>(entry.address));
			}

			template <typename from_type, typename string_type>
			void string_value_(entry_type &entry, char *destination, std::false_type){
				throw error_type::not_implemented;
			}

			template <typename target_type, typename string_type>
			entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, entry_type &operand, type_id_type string_id){
				auto right = operand.type->driver()->value<target_type>(operand);
				switch (operator_id){
				case operator_id_type::plus://Return string
					return concatenate_string_<string_type>(entry, operand, string_id);
				default:
					break;
				}

				return evaluate_boolean_<target_type>(entry, operator_id, operand);
			}

			template <typename target_type>
			entry_type *post_evaluate_(entry_type &entry, target_type value, bool assign, bool value_return = false){
				return oosl::common::controller::active->temporary_storage().add_scalar(value);
			}
		};
	}
}

#endif /* !OOSL_CHAR_DRIVER_H */
