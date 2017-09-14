#pragma once

#ifndef OOSL_CONTROLLER_H
#define OOSL_CONTROLLER_H

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include "../type/type_id.h"

namespace oosl{
	namespace type{
		class object;
	}

	namespace storage{
		struct entry;

		class object;
		class temporary;
	}

	namespace memory{
		class manager;
	}

	namespace driver{
		class object;
	}

	namespace common{
		struct interpreter_info;

		struct runtime_block_info;
		struct runtime_info;

		class error;
		class output_writer;

		class controller{
		public:
			typedef interpreter_info interpreter_info_type;
			typedef runtime_info runtime_info_type;

			typedef error error_type;
			typedef output_writer output_writer_type;

			typedef oosl::memory::manager memory_manager_type;
			typedef oosl::driver::object driver_object_type;

			typedef oosl::storage::entry storage_entry_type;
			typedef oosl::storage::object storage_object_type;
			typedef oosl::storage::temporary temporary_storage_type;

			typedef oosl::type::object type_object_type;
			typedef oosl::type::id type_id_type;

			typedef std::shared_ptr<type_object_type> type_object_ptr_type;
			typedef std::unordered_map<type_id_type, type_object_ptr_type> type_map_type;

			typedef std::function<void()> callback_type;

			enum class static_value_type{
				nil,
				indeterminate,
				false_,
				true_,
				nullptr_,
				nan_,
			};

			enum class driver_type{
				nil,
				boolean,
				byte,
				char_,
				numeric,
				array_,
				pointer,
				ref,
				dynamic,
				function,
				type,
				storage,
				node,
			};

			virtual ~controller();

			virtual bool exiting() = 0;

			virtual interpreter_info_type &interpreter_info() = 0;

			virtual runtime_info_type &runtime_info() = 0;

			virtual void on_exception(callback_type callback) = 0;

			virtual void on_exception_pop() = 0;

			virtual error_type &error() = 0;

			virtual output_writer_type &output_writer() = 0;

			virtual output_writer_type &error_output_writer() = 0;

			virtual memory_manager_type &memory() = 0;

			virtual storage_object_type &global_storage() = 0;

			virtual temporary_storage_type &temporary_storage() = 0;

			virtual type_object_ptr_type find_type(type_id_type id) = 0;

			virtual storage_entry_type *find_static_value(static_value_type type) = 0;

			virtual driver_object_type *find_driver(driver_type type) = 0;

			template <typename value_type>
			static std::string convert_base(value_type value, unsigned int base, std::size_t width = static_cast<std::size_t>(-1)){
				static auto digits = "0123456789abcdefghijklmnopqrstuvwxyz";
				if (base < 2u || width == 0u)//Invalid
					return "";

				auto strip_leading_zeros = false;
				if (width == static_cast<std::size_t>(-1)){//(sizeof(value_type) << 1)
					strip_leading_zeros = true;
					switch (base){
					case 32u://Base-32
						width = (sizeof(value_type) << 0);
						break;
					case 16u://Hex
						width = (sizeof(value_type) << 1);
						break;
					case 8u://Oct
						width = (sizeof(value_type) << 2);
						break;
					case 4u://Base-4
						width = (sizeof(value_type) << 3);
						break;
					case 2u://Bin
						width = (sizeof(value_type) << 4);
						break;
					default:
						width = ((base < 16u) ? (sizeof(value_type) << 4) : (sizeof(value_type) << 0));
						break;
					}
				}

				std::string rc(width, '0');
				for (std::size_t i = (width - 1u); value != static_cast<value_type>(0); --i, value /= base){
					rc[i] = digits[(value % base)];
					if (i == 0u)//Full width
						break;
				}

				if (strip_leading_zeros && rc.size() > 1u){//Strip leading zeros
					std::size_t leading_zeros = 0;
					for (std::size_t i = 0; i < rc.size(); ++i, ++leading_zeros){
						if (rc[i] != '0')//Leading terminated
							break;
					}

					if (leading_zeros != 0u)//Strip
						rc.erase(0, leading_zeros);
				}

				std::string prefix;
				switch (base){
				case 16u://Hex
					prefix = "0x";
					break;
				case 10u://Dec
					break;
				case 8u://Oct
					prefix = "0";
					break;
				case 2u://Bin
					prefix = "0b";
					break;
				default:
					prefix = (std::to_string(base) + "r");
					break;
				}

				return (prefix + rc);
			}

			template <typename string_type, typename value_type>
			static string_type real_to_string(value_type value){
				typedef char_type typename string_type::traits_type::char_type;

				auto string_value = to_string<string_type>(value);
				auto index = string_value.size();

				for (; index > 1u; --index){
					if (string_value[index - 2] == static_cast<char_type>('.') || string_value[index - 1] != static_cast<char_type>('0'))
						break;
				}

				if (index < string_value.size())
					string_value.erase(index);

				return string_value;
			}

			template <typename string_type, typename value_type>
			std::enable_if_t<std::is_same_v<string_type, std::string>, string_type> to_string(value_type value){
				return std::to_string(value);
			}

			template <typename string_type, typename value_type>
			std::enable_if_t<std::is_same_v<string_type, std::wstring>, string_type> to_string(value_type value){
				return std::to_wstring(value);
			}

			static controller *active;
		};
	}
}

#endif /* !OOSL_CONTROLLER_H */
