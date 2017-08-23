#pragma once

#ifndef OOSL_FUNCTION_TYPE_H
#define OOSL_FUNCTION_TYPE_H

#include "dynamic_type.h"

namespace oosl{
	namespace type{
		class function_type : public dynamic{
		public:
			function_type(ptr_type return_type, ptr_list_type &&parameters);

			virtual ~function_type();

			virtual object &function_return_type() override;

			virtual ptr_list_type &function_parameters() override;

			virtual std::string name() override;

			virtual std::string print() override;

			virtual int score(object &type) override;

			virtual int score(const storage_entry_list_type &entries) override;

			virtual bool is_function() override;

			virtual bool is_strong_function() override;

		protected:
			ptr_type return_type_;
			ptr_list_type parameters_;
		};
	}
}

#endif /* !OOSL_FUNCTION_TYPE_H */
