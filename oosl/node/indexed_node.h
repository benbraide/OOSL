#pragma once

#ifndef OOSL_INDEXED_NODE_H
#define OOSL_INDEXED_NODE_H

#include "node_object.h"

namespace oosl{
	namespace node{
		class indexed : public object{
		public:
			explicit indexed(const index_type &index);

			virtual ~indexed();

			virtual index_type &index() override;

		protected:
			index_type index_;
		};
	}
}

#endif /* !OOSL_INDEXED_NODE_H */
