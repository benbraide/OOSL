#pragma once

#ifndef OOSL_OPERATOR_ID_H
#define OOSL_OPERATOR_ID_H

namespace oosl{
	namespace common{
		enum class operator_id{
			comma,
			assignment,
			compound_bitwise_or,
			compound_bitwise_xor,
			compound_bitwise_and,
			compound_left_shift,
			compound_right_shift,
			compound_plus,
			compound_minus,
			compound_times,
			compound_divide,
			compound_modulus,
			ternary,
			relational_or,
			relational_and,
			ellipsis,
			bitwise_or,
			bitwise_xor,
			bitwise_and,
			variant_or,
			variant_and,
			like,
			equality,
			explicit_equality,
			inverse_equality,
			explicit_inverse_equality,
			less,
			less_or_equal,
			more_or_equal,
			more,
			left_shift,
			right_shift,
			plus,
			minus,
			times,
			divide,
			modulus,
			throw_,
			new_,
			delete_,
			sizeof_,
			typeof,
			bitwise_inverse,
			relational_not,
			increment,
			decrement,
			member_access,
			member_pointer_access,
			scope_resolution,
			call,
			index,
		};
	}
}

#endif /* !OOSL_OPERATOR_ID_H */
