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
#include "../common/structures.h"

#include "../storage/storage_object.h"
#include "../storage/temporary_storage.h"

#include "skipper.h"

#define OOSL_AST_JOIN_(x, y) x ## y
#define OOSL_AST_JOIN(x, y) OOSL_AST_JOIN_(x, y)

#define OOSL_AST_NAME(name) OOSL_AST_JOIN(name, _ast_type)
#define OOSL_AST_QNAME(name) OOSL_AST_JOIN(oosl::lexer::, OOSL_AST_NAME(name))

#define OOSL_AST_VISITOR_NAME(name) OOSL_AST_JOIN(OOSL_AST_NAME(name), _visitor)
#define OOSL_AST_VISITOR_QNAME(name) OOSL_AST_JOIN(OOSL_AST_QNAME(name), _visitor)

#define OOSL_AST_VISITOR_PROLOG_(name, type) OOSL_AST_VISITOR_NAME(name) : public boost::static_visitor<type>
#define OOSL_AST_VISITOR_PROLOG(name) OOSL_AST_VISITOR_PROLOG_(name, oosl::node::object::ptr_type)

#define OOSL_AST_TO_NODE_ ast_to_node
#define OOSL_AST_TO_QNODE_ OOSL_AST_JOIN(oosl::lexer::, ast_to_node)

#define OOSL_AST_TO_NODE_NAME(name) OOSL_AST_TO_NODE_<OOSL_AST_NAME(name)>
#define OOSL_AST_TO_NODE_QNAME(name) OOSL_AST_TO_QNODE_<OOSL_AST_QNAME(name)>

#define OOSL_AST_TO_NODE(name) template<> struct OOSL_AST_TO_NODE_NAME(name)

#define OOSL_AST_TO_NODE_GET(name, value) OOSL_AST_TO_NODE_NAME(name)::get(value)
#define OOSL_AST_TO_NODE_QGET(name, value) OOSL_AST_TO_NODE_QNAME(name)::get(value)

#define OOSL_AST_VISITOR_METHOD(arg)							\
oosl::node::object::ptr_type operator()(OOSL_AST_NAME(arg) &value) const{\
	return OOSL_AST_TO_NODE_GET(arg, value);					\
}

#define OOSL_AST_VISITOR_2(name, arg1, arg2)					\
struct OOSL_AST_VISITOR_PROLOG(name){							\
	OOSL_AST_VISITOR_METHOD(arg1)								\
	OOSL_AST_VISITOR_METHOD(arg2)								\
};

#define OOSL_AST_VISITOR_3(name, arg1, arg2, arg3)				\
struct OOSL_AST_VISITOR_PROLOG(name){							\
	OOSL_AST_VISITOR_METHOD(arg1)								\
	OOSL_AST_VISITOR_METHOD(arg2)								\
	OOSL_AST_VISITOR_METHOD(arg3)								\
};

#define OOSL_AST_VISITOR_4(name, arg1, arg2, arg3, arg4)		\
struct OOSL_AST_VISITOR_PROLOG(name){							\
	OOSL_AST_VISITOR_METHOD(arg1)								\
	OOSL_AST_VISITOR_METHOD(arg2)								\
	OOSL_AST_VISITOR_METHOD(arg3)								\
	OOSL_AST_VISITOR_METHOD(arg4)								\
};

#define OOSL_AST_VISITOR_5(name, arg1, arg2, arg3, arg4, arg5)	\
struct OOSL_AST_VISITOR_PROLOG(name){							\
	OOSL_AST_VISITOR_METHOD(arg1)								\
	OOSL_AST_VISITOR_METHOD(arg2)								\
	OOSL_AST_VISITOR_METHOD(arg3)								\
	OOSL_AST_VISITOR_METHOD(arg4)								\
	OOSL_AST_VISITOR_METHOD(arg5)								\
};

#define OOSL_AST_APPLY_VISITOR(name, value) boost::apply_visitor(OOSL_AST_VISITOR_NAME(name)(), value)

namespace oosl{
	namespace lexer{
		template <class ast_type>
		struct OOSL_AST_TO_NODE_;

		template <class visitor_type, class variant_type, typename... args_type>
		inline oosl::node::object::ptr_type apply_visitor(const variant_type &value, args_type &&... args){
			visitor_type visitor(std::forward<args_type>(args)...);
			return boost::apply_visitor(visitor, value);
		}
	}
}

#endif /* !OOSL_AST_H */
