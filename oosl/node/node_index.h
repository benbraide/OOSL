#pragma once

#ifndef OOSL_NODE_INDEX_H
#define OOSL_NODE_INDEX_H

#include <cstdlib>

namespace oosl{
	namespace node{
		struct index{
			std::size_t line;
			std::size_t column;
		};
	}
}

#endif /* !OOSL_NODE_INDEX_H */
