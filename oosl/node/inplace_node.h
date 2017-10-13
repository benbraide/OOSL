#pragma once

#ifndef OOSL_INPLACE_NODE_H
#define OOSL_INPLACE_NODE_H

#include "indexed_node.h"
#include "../type/type_mapper.h"

namespace oosl{
	namespace node{
		enum class inplace_target_type{
			destruct,
			index,
			is,
			print,
			key,
			eval,
			storage,
			type,
			count,
			traverse,
		};

		class numeric_inplace_value{
		public:
			typedef oosl::type::id type_id_type;

			virtual ~numeric_inplace_value() = default;

			virtual type_id_type type_id() = 0;

			virtual void converted_value(type_id_type id, char *out) = 0;

			template <typename target_type>
			target_type converted_value(){
				auto value = target_type();
				converted_value(oosl::type::mapper<target_type>::id, reinterpret_cast<char *>(&value));
				return value;
			}
		};

		template <class value_type, class enabled = void>
		class inplace_value;

		template <class value_type>
		class inplace_value<value_type, std::enable_if_t<std::is_void<value_type>::value>>{
		public:
			typedef value_type value_type;

			virtual ~inplace_value() = default;
		};

		template <class value_type>
		class inplace_value<value_type, std::enable_if_t<!std::is_void<value_type>::value && !std::is_integral<value_type>::value && !std::is_floating_point<value_type>::value>>{
		public:
			typedef value_type value_type;

			explicit inplace_value(const value_type &value)
				: value_(value){}

			virtual ~inplace_value() = default;

			virtual value_type &value(){
				return value_;
			}

		protected:
			value_type value_;
		};

		template <class value_type>
		class inplace_value<value_type, std::enable_if_t<!std::is_void<value_type>::value && (std::is_integral<value_type>::value || std::is_floating_point<value_type>::value)>> : public numeric_inplace_value{
		public:
			typedef value_type value_type;

			explicit inplace_value(value_type value)
				: value_(value){}

			virtual ~inplace_value() = default;

			virtual type_id_type type_id() override{
				return oosl::type::mapper<value_type>::id;
			}

			virtual void converted_value(type_id_type id, char *out) override{
				switch (id){
				case type_id_type::int8_:
					*reinterpret_cast<__int8 *>(out) = static_cast<__int8>(value_);
					break;
				case type_id_type::uint8_:
					*reinterpret_cast<unsigned __int8 *>(out) = static_cast<unsigned __int8>(value_);
					break;
				case type_id_type::int16_:
					*reinterpret_cast<__int16 *>(out) = static_cast<__int16>(value_);
					break;
				case type_id_type::uint16_:
					*reinterpret_cast<unsigned __int16 *>(out) = static_cast<unsigned __int16>(value_);
					break;
				case type_id_type::int32_:
					*reinterpret_cast<__int32 *>(out) = static_cast<__int32>(value_);
					break;
				case type_id_type::uint32_:
					*reinterpret_cast<unsigned __int32 *>(out) = static_cast<unsigned __int32>(value_);
					break;
				case type_id_type::int64_:
					*reinterpret_cast<__int64 *>(out) = static_cast<__int64>(value_);
					break;
				case type_id_type::uint64_:
					*reinterpret_cast<unsigned __int64 *>(out) = static_cast<unsigned __int64>(value_);
					break;
				case type_id_type::float_:
					*reinterpret_cast<float *>(out) = static_cast<float>(value_);
					break;
				case type_id_type::double_:
					*reinterpret_cast<double *>(out) = static_cast<double>(value_);
					break;
				case type_id_type::ldouble:
					*reinterpret_cast<long double *>(out) = static_cast<long double>(value_);
					break;
				default:
					break;
				}
			}

			virtual value_type &value(){
				return value_;
			}

		protected:
			value_type value_;
		};

		template <class value_type>
		class inplace : public object, public inplace_value<value_type>{
		public:
			typedef object base_type;

			typedef base_type::id_type id_type;
			typedef base_type::entry_type entry_type;

			typedef value_type value_type;
			typedef inplace_value<value_type> inplace_value_type;
			typedef inplace_target_type target_type;

			typedef std::function<bool(inplace &, target_type, void *)> callback_type;
			typedef std::function<bool(target_type, void *)> alt_callback_type;

			using base_type::echo;

			template <typename... args_type>
			inplace(id_type id, callback_type callback, args_type &&... args)
				: inplace_value_type(std::forward<args_type>(args)...), id_(id), callback_(callback){}

			template <typename... args_type>
			inplace(id_type id, alt_callback_type callback, args_type &&... args)
				: inplace(id, [callback](inplace &, target_type target, void *out){ return callback(target, out); }, std::forward<args_type>(args)...){}

			virtual ~inplace(){
				callback_(*this, target_type::destruct, nullptr);
			}

			virtual id_type id() override{
				return id_;
			}

			virtual bool is(id_type id) override{
				auto value = std::make_pair(id, false);
				return (callback_(*this, target_type::is, &value) ? value.second : base_type::is(id));
			}

			virtual void echo(output_writer_type &writer) override{
				if (!callback_(*this, target_type::print, &writer))
					base_type::echo(writer);
			}

			virtual std::string key() override{
				std::string value;
				return (callback_(*this, target_type::key, &value) ? value : base_type::key());
			}

			virtual storage_type *storage() override{
				storage_type *value;
				return (callback_(*this, target_type::storage, &value) ? value : base_type::storage());
			}

			virtual type_object_type *type() override{
				type_object_type *value;
				return (callback_(*this, target_type::type, &value) ? value : base_type::type());
			}

			virtual size_type count() override{
				size_type value;
				return (callback_(*this, target_type::count, &value) ? value : base_type::count());
			}

			virtual void traverse(traverser_type traverser) override{
				callback_(*this, target_type::traverse, &traverser);
			}

		protected:
			virtual entry_type *evaluate_() override{
				entry_type *value;
				return (callback_(*this, target_type::eval, &value) ? value : base_type::evaluate_());
			}

			id_type id_;
			callback_type callback_;
		};
	}
}

#endif /* !OOSL_INPLACE_NODE_H */
