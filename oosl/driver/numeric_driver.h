#pragma once

#ifndef OOSL_NUMERIC_DRIVER_H
#define OOSL_NUMERIC_DRIVER_H

#include "driver_object.h"

namespace oosl{
	namespace driver{
		class numeric : public object{
		public:
			using object::echo;
			using object::value;

			enum class evaluation_option_type : unsigned int{
				nil					= (0 << 0x0000),
				assign				= (1 << 0x0000),
				integral			= (1 << 0x0001),
			};

			virtual ~numeric();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil) override;

			virtual void echo(entry_type &entry, output_writer_type &writer) override;

			virtual void value(entry_type &entry, type_id_type to, char *destination) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, unary_operator_info_type &operator_info) override;

			virtual entry_type *evaluate_(entry_type &entry, binary_operator_info_type &operator_info, entry_type &operand) override;

			template <typename from_type>
			void value_from_(entry_type &entry, type_id_type to, char *destination){
				switch (to){
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
				return evaluate_<target_type>(entry, operator_id, operand.type->driver()->value<target_type>(operand));
			}

			template <typename target_type>
			entry_type *evaluate_(entry_type &entry, operator_id_type operator_id, target_type operand){
				switch (operator_id){
				case operator_id_type::plus:
					return post_evaluate_(entry, value<target_type>(entry) + operand);
				case operator_id_type::compound_plus:
					return post_evaluate_(entry, value<target_type>(entry) + operand, post_evaluation_type::assign);
				case operator_id_type::minus:
					return post_evaluate_(entry, value<target_type>(entry) - operand);
				case operator_id_type::compound_minus:
					return post_evaluate_(entry, value<target_type>(entry) - operand, post_evaluation_type::assign);
				case operator_id_type::times:
					return post_evaluate_(entry, value<target_type>(entry) * operand);
				case operator_id_type::compound_times:
					return post_evaluate_(entry, value<target_type>(entry) * operand, post_evaluation_type::assign);
				case operator_id_type::divide:
					if (operand == static_cast<target_type>(0))
						return oosl::common::controller::active->find_static_value(static_value_type::nan_);
					return post_evaluate_(entry, value<target_type>(entry) / operand);
				case operator_id_type::compound_divide:
					if (operand == static_cast<target_type>(0)){//NaN
						OOSL_SET(entry.attributes, attribute_type::nan_);
						return &entry;
					}
					return post_evaluate_(entry, value<target_type>(entry) / operand, post_evaluation_type::assign);
				default:
					break;
				}

				return evaluate_boolean_<target_type>(entry, operator_id, operand);
			}

			template <typename target_type>
			entry_type *evaluate_integral_(entry_type &entry, operator_id_type operator_id, entry_type &operand){
				return evaluate_integral_<target_type>(entry, operator_id, operand.type->driver()->value<target_type>(operand));
			}

			template <typename target_type>
			entry_type *evaluate_integral_(entry_type &entry, operator_id_type operator_id, target_type operand){
				switch (operator_id){
				case operator_id_type::modulus:
					if (operand == static_cast<target_type>(0))
						return oosl::common::controller::active->find_static_value(static_value_type::nan_);
					return post_evaluate_(entry, value<target_type>(entry) % operand);
				case operator_id_type::compound_modulus:
					if (operand == static_cast<target_type>(0)){//NaN
						OOSL_SET(entry.attributes, attribute_type::nan_);
						return &entry;
					}
					return post_evaluate_(entry, value<target_type>(entry) % operand, post_evaluation_type::assign);
				case operator_id_type::left_shift:
					return post_evaluate_(entry, value<target_type>(entry) << operand);
				case operator_id_type::compound_left_shift:
					return post_evaluate_(entry, value<target_type>(entry) << operand, post_evaluation_type::assign);
				case operator_id_type::right_shift:
					return post_evaluate_(entry, value<target_type>(entry) >> operand);
				case operator_id_type::compound_right_shift:
					return post_evaluate_(entry, value<target_type>(entry) >> operand, post_evaluation_type::assign);
				case operator_id_type::bitwise_and:
					return post_evaluate_(entry, value<target_type>(entry) & operand);
				case operator_id_type::compound_bitwise_and:
					return post_evaluate_(entry, value<target_type>(entry) & operand, post_evaluation_type::assign);
				case operator_id_type::bitwise_or:
					return post_evaluate_(entry, value<target_type>(entry) | operand);
				case operator_id_type::compound_bitwise_or:
					return post_evaluate_(entry, value<target_type>(entry) | operand, post_evaluation_type::assign);
				case operator_id_type::bitwise_xor:
					return post_evaluate_(entry, value<target_type>(entry) ^ operand);
				case operator_id_type::compound_bitwise_xor:
					return post_evaluate_(entry, value<target_type>(entry) ^ operand, post_evaluation_type::assign);
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
						return post_evaluate_(entry, value<target_type>(entry));
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
						return post_evaluate_(entry, -value<target_type>(entry));
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
						return post_evaluate_(entry, ~value<target_type>(entry));
					case operator_id_type::decrement:
						return post_evaluate_(entry, (value<target_type>(entry) - static_cast<target_type>(1)), post_evaluation_type::assign);
					case operator_id_type::increment:
						return post_evaluate_(entry, (value<target_type>(entry) + static_cast<target_type>(1)), post_evaluation_type::assign);
					default:
						break;
					}
				}
				else{//Right
					switch (operator_id){
					case operator_id_type::decrement:
						return post_evaluate_(entry, (value<target_type>(entry) - static_cast<target_type>(1)), (post_evaluation_type::assign | post_evaluation_type::value_return));
					case operator_id_type::increment:
						return post_evaluate_(entry, (value<target_type>(entry) + static_cast<target_type>(1)), (post_evaluation_type::assign | post_evaluation_type::value_return));
					default:
						break;
					}
				}

				return call_evaluate_<target_type>(entry, operator_id, left);
			}

			template <typename target_type>
			std::enable_if_t<std::is_unsigned_v<target_type>, entry_type *> call_evaluate_(entry_type &entry, operator_id_type operator_id, bool left){
				return evaluate_<target_type>(entry, operator_id, left);
			}

			template <typename target_type>
			std::enable_if_t<!std::is_unsigned_v<target_type>, entry_type *> call_evaluate_(entry_type &entry, operator_id_type operator_id, bool left){
				return evaluate_signed_<target_type>(entry, operator_id, left);
			}

			std::string suffix_(entry_type &entry);

			static bool is_(evaluation_option_type left, evaluation_option_type right);
		};

		OOSL_MAKE_OPERATORS(numeric::evaluation_option_type);
	}
}

#endif /* !OOSL_NUMERIC_DRIVER_H */
