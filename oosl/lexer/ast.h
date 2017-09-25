#pragma once

#ifndef OOSL_AST_H
#define OOSL_AST_H

#include <string>
#include <vector>
#include <variant>

#include <boost/optional.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "../node/node_object.h"
#include "../common/controller.h"

#include "../storage/storage_object.h"
#include "../storage/temporary_storage.h"

#define OOSL_AST_JOIN_(x, y) x ## y
#define OOSL_AST_JOIN(x, y) OOSL_AST_JOIN_(x, y)

#define OOSL_AST_NAME(name) OOSL_AST_JOIN(name, _ast_type)
#define OOSL_AST_QNAME(name) OOSL_AST_JOIN(oosl::lexer::, OOSL_AST_NAME(name))

namespace oosl{
	namespace lexer{
		template <class visitor_type, class variant_type, typename... args_type>
		inline oosl::node::object::ptr_type apply_visitor(variant_type &value, args_type &&... args){
			visitor_type visitor(std::forward<args_type>(args)...);
			return boost::apply_visitor(visitor, value);
		}
	}
}

#endif /* !OOSL_AST_H */
