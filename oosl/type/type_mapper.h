#pragma once

#ifndef OOSL_TYPE_MAPPER_H
#define OOSL_TYPE_MAPPER_H

#include "custom_types.h"
#include "type_object.h"

namespace oosl{
	namespace type{
		template <class value_type>
		struct mapper;

		template <>
		struct mapper<std::nullptr_t>{
			static const type::id id = type::id::nullptr_;
		};

		template <>
		struct mapper<void>{
			static const type::id id = type::id::void_;
		};

		template <>
		struct mapper<bool_type>{
			static const type::id id = type::id::bool_;
		};

		template <>
		struct mapper<wchar_t>{
			static const type::id id = type::id::wchar_;
		};

		template <>
		struct mapper<__int8>{
			static const type::id id = type::id::int8_;
		};

		template <>
		struct mapper<unsigned __int8>{
			static const type::id id = type::id::uint8_;
		};

		template <>
		struct mapper<__int16>{
			static const type::id id = type::id::int16_;
		};

		template <>
		struct mapper<unsigned __int16>{
			static const type::id id = type::id::uint16_;
		};

		template <>
		struct mapper<__int32>{
			static const type::id id = type::id::int32_;
		};

		template <>
		struct mapper<unsigned __int32>{
			static const type::id id = type::id::uint32_;
		};

		template <>
		struct mapper<__int64>{
			static const type::id id = type::id::int64_;
		};

		template <>
		struct mapper<unsigned __int64>{
			static const type::id id = type::id::uint64_;
		};

		template <>
		struct mapper<int128_type>{
			static const type::id id = type::id::int128_;
		};

		template <>
		struct mapper<uint128_type>{
			static const type::id id = type::id::uint128_;
		};

		template <>
		struct mapper<float>{
			static const type::id id = type::id::float_;
		};

		template <>
		struct mapper<double>{
			static const type::id id = type::id::double_;
		};

		template <>
		struct mapper<long double>{
			static const type::id id = type::id::ldouble;
		};

		template <>
		struct mapper<string_type>{
			static const type::id id = type::id::string_;
		};

		template <>
		struct mapper<wstring_type>{
			static const type::id id = type::id::wstring_;
		};
	}
}

#endif /* !OOSL_TYPE_MAPPER_H */
