#pragma once

#ifndef OOSL_DRIVER_OBJECT_H
#define OOSL_DRIVER_OBJECT_H

#include "../type/type_mapper.h"
#include "../type/custom_types.h"
#include "../type/pointer_type.h"

#include "../common/operator_info.h"
#include "../common/output_writer.h"

#include "../node/node_object.h"
#include "../storage/temporary_storage.h"

namespace oosl{
	namespace driver{
		class object{
		public:
			typedef oosl::type::object type_object_type;
			typedef oosl::type::id type_id_type;
			typedef oosl::type::bool_type bool_type;

			typedef type_object_type::ptr_type type_ptr_type;
			typedef oosl::node::object::ptr_type node_ptr_type;

			typedef oosl::memory::value_dependency<type_ptr_type> type_value_dependency_type;
			typedef oosl::memory::block::attribute_type memory_attribute_type;

			typedef common::error_codes error_type;
			typedef common::output_writer output_writer_type;
			typedef output_writer_type::write_option_type write_option_type;

			typedef common::controller controller_type;
			typedef controller_type::static_value_type static_value_type;
			typedef controller_type::output_writer_key_type output_writer_key_type;

			typedef common::operator_id operator_id_type;
			typedef common::operator_info operator_info_type;

			typedef storage::entry entry_type;
			typedef entry_type::attribute_type attribute_type;

			typedef std::size_t size_type;
			typedef unsigned __int8 uint8_type;
			typedef unsigned __int64 uint64_type;

			enum class cast_option_type : unsigned int{
				nil				= (0 << 0x0000),
				reinterpret		= (1 << 0x0000),
				is_explicit		= (1 << 0x0001),
				ref				= (1 << 0x0002),
				val				= (1 << 0x0003),
				make_copy		= (1 << 0x0004),
			};

			enum class post_evaluation_type : unsigned int{
				nil					= (0 << 0x0000),
				assign				= (1 << 0x0000),
				value_return		= (1 << 0x0001),
				byte				= (1 << 0x0002),
			};

			virtual ~object();

			virtual entry_type *cast(entry_type &entry, type_object_type &type, cast_option_type options = cast_option_type::nil);

			virtual type_object_type *type(entry_type &entry);

			virtual size_type size(entry_type &entry);

			virtual uint64_type address(entry_type &entry);

			virtual entry_type *linked(entry_type &entry);

			virtual entry_type *duplicate(entry_type &entry);

			virtual attribute_type attributes(entry_type &entry);

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info);

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info, entry_type &operand);

			virtual entry_type *evaluate(entry_type &entry, operator_info_type &operator_info, node_ptr_type &operand);

			virtual entry_type *assign(entry_type &entry, entry_type &value);

			virtual void initialize(entry_type &entry);

			virtual bool is_lval(entry_type &entry);

			virtual bool is_ref(entry_type &entry);

			virtual bool is_uninitialized(entry_type &entry);

			virtual bool is_tls(entry_type &entry);

			virtual bool is_nan(entry_type &entry);

			virtual bool is_void(entry_type &entry);

			virtual void echo(entry_type &entry);

			virtual void echo(entry_type &entry, output_writer_type &writer);

			virtual void value(entry_type &entry, type_id_type to, char *destination);

			template <typename target_type>
			void value(entry_type &entry, target_type &val){
				value(entry, type::mapper<target_type>::id, reinterpret_cast<char *>(&val));
			}

			template <typename target_type>
			target_type value(entry_type &entry){
				target_type val;
				value(entry, val);
				return val;
			}

		protected:
			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info);

			virtual entry_type *evaluate_(entry_type &entry, operator_info_type &operator_info, entry_type &operand);

			virtual entry_type *assign_(entry_type &entry, entry_type &value);

			template <typename target_type>
			entry_type *evaluate_boolean_(entry_type &entry, operator_id_type operator_id, entry_type &operand){
				return evaluate_boolean_<target_type>(entry, operator_id, operand.type->driver()->value<target_type>(operand));
			}

			template <typename target_type>
			entry_type *evaluate_boolean_(entry_type &entry, operator_id_type operator_id, target_type operand){
				switch (operator_id){
				case operator_id_type::less:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) < operand) ? bool_type::true_ : bool_type::false_);
				case operator_id_type::less_or_equal:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) <= operand) ? bool_type::true_ : bool_type::false_);
				case operator_id_type::more_or_equal:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) >= operand) ? bool_type::true_ : bool_type::false_);
				case operator_id_type::more:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) > operand) ? bool_type::true_ : bool_type::false_);
				default:
					break;
				}

				return evaluate_equality_<target_type>(entry, operator_id, operand);
			}

			template <typename target_type>
			entry_type *evaluate_equality_(entry_type &entry, operator_id_type operator_id, entry_type &operand){
				return evaluate_equality_<target_type>(entry, operator_id, operand.type->driver()->value<target_type>(operand));
			}

			template <typename target_type>
			entry_type *evaluate_equality_(entry_type &entry, operator_id_type operator_id, target_type operand){
				switch (operator_id){
				case operator_id_type::explicit_equality:
				case operator_id_type::equality:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) == operand) ? bool_type::true_ : bool_type::false_);
				case operator_id_type::explicit_inverse_equality:
				case operator_id_type::inverse_equality:
					return oosl::common::controller::active->temporary_storage().add_scalar((value<target_type>(entry) != operand) ? bool_type::true_ : bool_type::false_);
				default:
					break;
				}

				throw error_type::unhandled_operator;
			}

			template <typename string_type>
			entry_type *concatenate_string_(entry_type &entry, entry_type &operand, type_id_type id){
				typedef typename string_type::traits_type::char_type char_type;
				string_type left, right;

				value(entry, id, reinterpret_cast<char *>(&left));
				operand.type->driver()->value(operand, id, reinterpret_cast<char *>(&right));

				return oosl::common::controller::active->temporary_storage().add_scalar(left + right);
			}

			template <typename target_type>
			entry_type *post_evaluate_(entry_type &entry, target_type value, post_evaluation_type options = post_evaluation_type::nil){
				if (!OOSL_IS(options, post_evaluation_type::assign)){//Temporary value
					if (!OOSL_IS(options, post_evaluation_type::byte))
						return oosl::common::controller::active->temporary_storage().add_scalar(value);
					return oosl::common::controller::active->temporary_storage().add_scalar(static_cast<uint8_type>(value), oosl::common::controller::active->find_type(type_id_type::byte_));
				}

				if (!is_lval(entry))
					throw error_type::rval_assignment;

				auto block = oosl::common::controller::active->memory().find_block(entry.address);
				if (block == nullptr)//Error
					throw error_type::invalid_address;

				memcpy(block->ptr, &value, block->size);
				if (OOSL_IS(options, post_evaluation_type::value_return)){//Return value
					if (!OOSL_IS(options, post_evaluation_type::byte))
						return oosl::common::controller::active->temporary_storage().add_scalar(value, entry.type);
					return oosl::common::controller::active->temporary_storage().add_scalar(static_cast<uint8_type>(value), oosl::common::controller::active->find_type(type_id_type::byte_));
				}

				return &entry;
			}
		};

		OOSL_MAKE_OPERATORS(object::cast_option_type);
		OOSL_MAKE_OPERATORS(object::post_evaluation_type);
	}
}

#endif /* !OOSL_DRIVER_OBJECT_H */
