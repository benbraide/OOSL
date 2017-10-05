#pragma once

#ifndef OOSL_OPERATOR_ID_H
#define OOSL_OPERATOR_ID_H

namespace oosl{
	namespace common{
		enum class operator_id{
			nil,
			comma,
			assignment,
			compound_plus,
			compound_minus,
			compound_times,
			compound_divide,
			compound_modulus,
			compound_bitwise_or,
			compound_bitwise_xor,
			compound_bitwise_and,
			compound_left_shift,
			compound_right_shift,
			plus,
			minus,
			times,
			divide,
			modulus,
			bitwise_or,
			bitwise_xor,
			bitwise_and,
			left_shift,
			right_shift,
			bitwise_inverse,
			ternary,
			relational_or,
			relational_and,
			ellipsis,
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
			throw_,
			new_,
			delete_,
			sizeof_,
			typeof,
			relational_not,
			increment,
			decrement,
			member_access,
			member_pointer_access,
			scope_resolution,
			call,
			index,
			unknown,
		};
	}
}

#endif /* !OOSL_OPERATOR_ID_H */
