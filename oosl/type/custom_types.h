#pragma once

#ifndef OOSL_CUSTOM_TYPES_H
#define OOSL_CUSTOM_TYPES_H

namespace oosl{
	namespace type{
		enum class bool_type : char{
			indeterminate	= '\x0',
			false_			= '\x1',
			true_			= '\x2',
		};

		enum class int128_type : char{
			unused
		};

		enum class uint128_type : char{
			unused
		};

		enum class string_type : char{
			unused
		};

		enum class wstring_type : char{
			unused
		};
	}
}

#endif /* !OOSL_CUSTOM_TYPES_H */
