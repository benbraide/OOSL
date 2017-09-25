#pragma once

#ifndef OOSL_RAII_H
#define OOSL_RAII_H

#include <functional>

namespace oosl{
	namespace common{
		class default_raii_base{};

		template <typename value_type>
		class value_raii_base{
		public:
			typedef value_type value_type;

			explicit value_raii_base(const value_type &value)
				: value_(value){}

			const value_type &value() const{
				return value_;
			}

		protected:
			value_type value_;
		};

		template <class base_type>
		class raii : public base_type{
		public:
			typedef base_type base_type;
			typedef std::function<void(raii &)> callback_type;

			template <typename... args_types>
			raii(callback_type init, callback_type uninit, args_types &&... args)
				: base_type(std::forward<args_types>(args)...), uninit_(uninit){
				init(*this);
			}

			~raii(){
				uninit_(*this);
			}

		private:
			callback_type uninit_;
		};

		template <class target_type, class value_type = target_type>
		class raii_state : public value_raii_base<value_type>{
		public:
			typedef target_type target_type;
			typedef value_raii_base<value_type> base_type;

			raii_state(target_type &target, const value_type &value)
				: base_type(target), target_(&target){
				target = value;//Set value
			}

			~raii_state(){
				*target_ = base_type::value_;//Restore value
			}

		protected:
			target_type *target_;
		};
	}
}

#endif /* !OOSL_RAII_H */
