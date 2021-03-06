#pragma once

#ifndef OOSL_NODE_ID_H
#define OOSL_NODE_ID_H

namespace oosl{
	namespace node{
		enum class id{
			nil,
			literal_,
			constant,
			integral,
			range,
			operator_,
			identifier,
			qualified,
			type,
			enum_,
			union_,
			struct_,
			class_,
			namespace_,
			type_with_storage,
			typedef_,
			declaration,
			function,
			type_cast,
			placeholder,
			decl_type,
			operator_value_,
			echo,
			return_,
			break_,
			continue_,
			case_,
			default_,
			if_,
			unless,
			conditional,
			while_,
			until,
			do_while,
			for_,
			try_,
			catch_,
			finally_,
			throw_,
			using_,
			array_type,
			pointer_type,
			function_type,
			variadic_type,
			auto_type,
			void_type,
			collection,
			statement,
			block,
			initialization,
			uniform_initialization,
			list,
			identifier_compatible,
			type_compatible,
		};
	}
}

#endif /* !OOSL_NODE_ID_H */
