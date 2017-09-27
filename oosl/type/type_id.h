#pragma once

#ifndef OOSL_TYPE_ID_H
#define OOSL_TYPE_ID_H

namespace oosl{
	namespace type{
		enum class id{
			nil,
			auto_,
			any_,
			undefined,
			void_,
			bool_,
			byte_,
			char_,
			wchar_,
			int8_,
			uint8_,
			int16_,
			uint16_,
			int32_,
			uint32_,
			int64_,
			uint64_,
			int128_,
			uint128_,
			float_,
			double_,
			ldouble,
			string_,
			wstring_,
			array_,
			pointer_,
			function_,
			nullptr_,
			type_,
			node_,
			storage_,
			compile_,
			runtime_,
			variant_,
			enum_,
			union_,
			struct_,
			class_,
			nan_,
		};
	}
}

#endif /* !OOSL_TYPE_ID_H */
