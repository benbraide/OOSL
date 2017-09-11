#pragma once

#ifndef OOSL_NUMERIC_DRIVER_H
#define OOSL_NUMERIC_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class numeric : public object{
		public:
			using object::value;

			enum class evaluation_option_type : unsigned int{
				nil					= (0 << 0x0000),
				assign				= (1 << 0x0000),
				integral			= (1 << 0x0001),
			};

			virtual ~numeric();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, unary_operator_info_type &operator_info) override;

			virtual entry_type *evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand) override;

			template <typename from_type>
			void value_from_(entry_type &entry, type_id_type to, char *destination){
				switch (to){
				case type_id_type::char_:
					return value_<from_type, char>(entry, to, destination);
				case type_id_type::wchar_:
					return value_<from_type, wchar_t>(entry, to, destination);
				case type_id_type::int8_:
					return value_<from_type, __int8>(entry, to, destination);
				case type_id_type::uint8_:
					return value_<from_type, unsigned __int8>(entry, to, destination);
				case type_id_type::int16_:
					return value_<from_type, __int16>(entry, to, destination);
				case type_id_type::uint16_:
					return value_<from_type, unsigned __int16>(entry, to, destination);
				case type_id_type::int32_:
					return value_<from_type, __int32>(entry, to, destination);
				case type_id_type::uint32_:
					return value_<from_type, unsigned __int32>(entry, to, destination);
				case type_id_type::int64_:
					return value_<from_type, __int64>(entry, to, destination);
				case type_id_type::uint64_:
					return value_<from_type, unsigned __int64>(entry, to, destination);
				/*case type_id_type::int128_:
					return value_<from_type, __int128>(entry, to, destination);
				case type_id_type::uint128_:
					return value_<from_type, unsigned __int128>(entry, to, destination);*/
				case type_id_type::float_:
					return value_<from_type, float>(entry, to, destination);
				case type_id_type::double_:
					return value_<from_type, double>(entry, to, destination);
				case type_id_type::ldouble:
					return value_<from_type, long double>(entry, to, destination);
				case type_id_type::string_:
					*reinterpret_cast<std::string *>(destination) = std::to_string(oosl::common::controller::active->memory().read<from_type>(entry.address));
					return;
				case type_id_type::wstring_:
					*reinterpret_cast<std::wstring *>(destination) = std::to_wstring(oosl::common::controller::active->memory().read<from_type>(entry.address));
					return;
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

			template <typename target_type>
			entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, entry_type &operand){
				auto right = operand.type->driver()->value<target_type>(operand);
				switch (operator_id){
				case operator_id_type::plus:
					return post_evaluate_(entry, value<target_type>(entry) + right, false);
				case operator_id_type::compound_plus:
					return post_evaluate_(entry, value<target_type>(entry) + right, true);
				case operator_id_type::minus:
					return post_evaluate_(entry, value<target_type>(entry) - right, false);
				case operator_id_type::compound_minus:
					return post_evaluate_(entry, value<target_type>(entry) - right, true);
				case operator_id_type::times:
					return post_evaluate_(entry, value<target_type>(entry) * right, false);
				case operator_id_type::compound_times:
					return post_evaluate_(entry, value<target_type>(entry) * right, true);
				case operator_id_type::divide:
					if (right == static_cast<target_type>(0))
						return nullptr;//#TODO: Return NaN
					return post_evaluate_(entry, value<target_type>(entry) / right, false);
				case operator_id_type::compound_divide:
					if (right == static_cast<target_type>(0))
						return nullptr;//#TODO: Return NaN
					return post_evaluate_(entry, value<target_type>(entry) / right, true);
				case operator_id_type::less:
					return post_evaluate_(entry, (value<target_type>(entry) < right) ? bool_type::true_ : bool_type::false_, false);
				case operator_id_type::less_or_equal:
					return post_evaluate_(entry, (value<target_type>(entry) <= right) ? bool_type::true_ : bool_type::false_, false);
				case operator_id_type::equality:
					return post_evaluate_(entry, (value<target_type>(entry) == right) ? bool_type::true_ : bool_type::false_, false);
				case operator_id_type::inverse_equality:
					return post_evaluate_(entry, (value<target_type>(entry) != right) ? bool_type::true_ : bool_type::false_, false);
				case operator_id_type::more_or_equal:
					return post_evaluate_(entry, (value<target_type>(entry) >= right) ? bool_type::true_ : bool_type::false_, false);
				case operator_id_type::more:
					return post_evaluate_(entry, (value<target_type>(entry) > right) ? bool_type::true_ : bool_type::false_, false);
				default:
					break;
				}

				throw error_type::not_implemented;
			}

			template <typename target_type>
			entry_type *evaluate_integral_(entry_type &entry, operator_id_type operator_id, entry_type &operand){
				if (!operand.type->driver()->type(operand)->is_integral())
					return evaluate_<target_type>(entry, operator_id, operand);

				auto right = operand.type->driver()->value<target_type>(operand);
				switch (operator_id){
				case operator_id_type::modulus:
					if (right == static_cast<target_type>(0))
						return nullptr;//#TODO: Return NaN
					return post_evaluate_(entry, value<target_type>(entry) % right, false);
				case operator_id_type::compound_modulus:
					if (right == static_cast<target_type>(0))
						return nullptr;//#TODO: Return NaN
					return post_evaluate_(entry, value<target_type>(entry) % right, true);
				case operator_id_type::left_shift:
					return post_evaluate_(entry, value<target_type>(entry) << right, false);
				case operator_id_type::compound_left_shift:
					return post_evaluate_(entry, value<target_type>(entry) << right, true);
				case operator_id_type::right_shift:
					return post_evaluate_(entry, value<target_type>(entry) >> right, false);
				case operator_id_type::compound_right_shift:
					return post_evaluate_(entry, value<target_type>(entry) >> right, true);
				case operator_id_type::bitwise_and:
					return post_evaluate_(entry, value<target_type>(entry) & right, false);
				case operator_id_type::compound_bitwise_and:
					return post_evaluate_(entry, value<target_type>(entry) & right, true);
				case operator_id_type::bitwise_or:
					return post_evaluate_(entry, value<target_type>(entry) | right, false);
				case operator_id_type::compound_bitwise_or:
					return post_evaluate_(entry, value<target_type>(entry) | right, true);
				case operator_id_type::bitwise_xor:
					return post_evaluate_(entry, value<target_type>(entry) ^ right, false);
				case operator_id_type::compound_bitwise_xor:
					return post_evaluate_(entry, value<target_type>(entry) ^ right, true);
				default:
					break;
				}

				return evaluate_<target_type>(entry, operator_id, operand);
			}

			template <typename target_type>
			entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, bool left){
				if (left){
					switch (operator_id){
					case operator_id_type::plus:
						return post_evaluate_(entry, value<target_type>(entry), false);
					default:
						break;
					}
				}

				throw error_type::not_implemented;
			}

			template <typename target_type>
			entry_type *evaluate_signed_(entry_type &entry, operator_id_type operator_id, bool left){
				if (left){
					switch (operator_id){
					case operator_id_type::minus:
						return post_evaluate_(entry, -value<target_type>(entry), false);
					default:
						break;
					}
				}

				return evaluate_<target_type>(entry, operator_id, left);
			}

			template <typename target_type>
			entry_type *evaluate_integral_(entry_type &entry, operator_id_type operator_id, bool left){
				if (left){
					switch (operator_id){
					case operator_id_type::bitwise_inverse:
						return post_evaluate_(entry, ~value<target_type>(entry), false);
					case operator_id_type::decrement:
						return post_evaluate_(entry, (value<target_type>(entry) - static_cast<target_type>(1)), true);
					case operator_id_type::increment:
						return post_evaluate_(entry, (value<target_type>(entry) + static_cast<target_type>(1)), true);
					default:
						break;
					}
				}
				else{//Right
					switch (operator_id){
					case operator_id_type::decrement:
						return post_evaluate_(entry, (value<target_type>(entry) - static_cast<target_type>(1)), true, true);
					case operator_id_type::increment:
						return post_evaluate_(entry, (value<target_type>(entry) + static_cast<target_type>(1)), true, true);
					default:
						break;
					}
				}

				return call_evaluate_<target_type>(entry, operator_id, left);
			}

			template <typename target_type>
			entry_type *post_evaluate_(entry_type &entry, target_type value, bool assign, bool value_return = false){
				if (!assign)//Temporary value
					return oosl::common::controller::active->temporary_storage().add_scalar(value);

				if (!is_lval(entry))
					throw error_type::rval_assignment;

				auto block = oosl::common::controller::active->memory().find_block(entry.address);
				if (block == nullptr)//Error
					throw error_type::invalid_address;

				memcpy(block->ptr, &value, block->size);
				if (value_return)//Return value
					return oosl::common::controller::active->temporary_storage().add_scalar(value);

				return &entry;
			}

			template <typename target_type>
			std::enable_if_t<std::is_unsigned_v<target_type>, entry_type *> call_evaluate_(entry_type &entry, operator_id_type operator_id, bool left){
				return evaluate_<target_type>(entry, operator_id, left);
			}

			template <typename target_type>
			std::enable_if_t<!std::is_unsigned_v<target_type>, entry_type *> call_evaluate_(entry_type &entry, operator_id_type operator_id, bool left){
				return evaluate_signed_<target_type>(entry, operator_id, left);
			}

			template <typename string_type>
			entry_type *concatenate_string_(entry_type &entry, entry_type &operand, type_id_type id){
				typedef typename string_type::traits_type::char_type char_type;
				string_type left, right;

				value(entry, id, reinterpret_cast<char *>(&left));
				operand.type->driver()->value(operand, id, reinterpret_cast<char *>(&right));
				left += right;

				auto new_entry = oosl::common::controller::active->temporary_storage().add(static_cast<size_type>((left.size() + 1) * sizeof(char_type)));
				auto block = oosl::common::controller::active->memory().find_block(new_entry->address);
				if (block == nullptr)//Error
					throw error_type::out_of_memory;

				memcpy(block->ptr, (char *)left.c_str(), block->size);//Copy bytes
				new_entry->type = oosl::common::controller::active->find_type(id);
				OOSL_SET(block->attributes, memory_attribute_type::immutable);

				return new_entry;
			}

			static bool is_(evaluation_option_type left, evaluation_option_type right);
		};

		OOSL_MAKE_OPERATORS(numeric::evaluation_option_type);
	}
}

#endif /* !OOSL_NUMERIC_DRIVER_H */
