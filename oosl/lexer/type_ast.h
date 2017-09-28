#pragma once

#ifndef OOSL_TYPE_AST_H
#define OOSL_TYPE_AST_H

#include "../node/inplace_node.h"

#include "ast.h"
#include "general_grammar.h"

namespace oosl{
	namespace lexer{
		enum class type_specifier{
			nil,
			static_,
			thread_local_,
		};

		enum class ref_type{
			nil,
			ref_,
			rval_ref,
		};

		enum class primitive_type{
			nil,
			any_,
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
			nullptr_,
		};

		struct OOSL_AST_NAME(type_specifier){
			typedef boost::variant<
				std::vector<type_specifier>,
				type_specifier
			> value_type;
			value_type value;
		};

		struct OOSL_AST_NAME(primitive_type){
			primitive_type value;
		};

		struct OOSL_AST_NAME(type_compatible){
			typedef boost::variant<
				OOSL_AST_NAME(primitive_type),
				OOSL_AST_NAME(qualified),
				OOSL_AST_NAME(identifier)
				/*OOSL_AST_NAME(placeholder)*/
			> value_type;
			value_type value;
		};

		struct OOSL_AST_NAME(type_extender){
			boost::optional<std::string> pointer;
			boost::optional<ref_type> ref_;
		};

		struct OOSL_AST_NAME(primitive_type_extended){
			OOSL_AST_NAME(primitive_type) value;
			OOSL_AST_NAME(type_extender) extender;
		};

		struct OOSL_AST_NAME(type_extended){
			OOSL_AST_NAME(type_compatible) value;
			OOSL_AST_NAME(type_extender) extender;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(primitive_type),
	(oosl::lexer::primitive_type, value)
)

BOOST_FUSION_ADAPT_STRUCT(
	OOSL_AST_QNAME(type_specifier),
	(OOSL_AST_QNAME(type_specifier)::value_type, value)
)

#endif /* !OOSL_TYPE_AST_H */
