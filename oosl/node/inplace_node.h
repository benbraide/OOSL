#pragma once

#ifndef OOSL_INPLACE_NODE_H
#define OOSL_INPLACE_NODE_H

#include "indexed_node.h"

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
		class inplace_value<value_type, std::enable_if_t<!std::is_void<value_type>::value>>{
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
		class inplace : public indexed, public inplace_value<value_type>{
		public:
			typedef indexed base_type;

			typedef base_type::id_type id_type;
			typedef base_type::entry_type entry_type;

			typedef value_type value_type;
			typedef inplace_value<value_type> inplace_value_type;
			typedef inplace_target_type target_type;

			typedef std::function<bool(inplace &, target_type, void *)> callback_type;
			typedef std::function<bool(target_type, void *)> alt_callback_type;

			template <typename... args_type>
			inplace(id_type id, const index_type &index, callback_type callback, args_type &&... args)
				: base_type(index), inplace_value_type(std::forward<args_type>(args)...), id_(id), callback_(callback){}

			template <typename... args_type>
			inplace(id_type id, const index_type &index, alt_callback_type callback, args_type &&... args)
				: inplace(id, index, [callback](inplace &, target_type target, void *out){ return callback(target, out); }, std::forward<args_type>(args)...){}

			virtual ~inplace(){
				callback_(*this, target_type::destruct, nullptr);
			}

			virtual id_type id() override{
				return id_;
			}

			virtual index_type &index() override{
				index_type *value;
				return (callback_(*this, target_type::index, &value) ? *value : base_type::index());
			}

			virtual bool is(id_type id) override{
				auto value = std::make_pair(id, false);
				return (callback_(*this, target_type::is, &value) ? value.second : base_type::is(id));
			}

			virtual std::string print() override{
				std::string value;
				return (callback_(*this, target_type::print, &value) ? value : base_type::print());
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
