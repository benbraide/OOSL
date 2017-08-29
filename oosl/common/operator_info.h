#pragma once

#ifndef OOSL_OPERATOR_INFO_H
#define OOSL_OPERATOR_INFO_H

#include <string>

#include "operator_id.h"

namespace oosl{
	namespace common{
		struct binary_operator_info{
			operator_id id;
			std::string value;
		};

		struct unary_operator_info{
			bool is_left;
			operator_id id;
			std::string value;
		};
	}
}

#endif /* !OOSL_OPERATOR_INFO_H */
