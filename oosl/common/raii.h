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
	}
}

#endif /* !OOSL_RAII_H */
