#pragma once

#ifndef OOSL_LIST_NODE_H
#define OOSL_LIST_NODE_H

#include <vector>

#include "node_object.h"

namespace oosl{
	namespace node{
		class list : public object{
		public:
			typedef std::vector<ptr_type> ptr_list_type;

			using object::echo;

			list(ptr_list_type &&list, const std::string &delimiter);

			virtual ~list();

			virtual id_type id() override;

			virtual void echo(output_writer_type &writer) override;

			virtual void traverse(traverser_type traverser) override;

		protected:
			ptr_list_type list_;
			std::string delimiter_;
		};
	}
}

#endif /* !OOSL_LIST_NODE_H */
