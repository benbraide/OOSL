#pragma once

#ifndef OOSL_EXPRESSION_NODE_H
#define OOSL_EXPRESSION_NODE_H

#include "../common/operator_info.h"

#include "node_object.h"

namespace oosl{
	namespace node{
		struct expression_helper{
			typedef oosl::common::operator_id operator_id_type;

			static std::string known_operator_name(operator_id_type id);
		};

		class unary_expression : public object{
		public:
			typedef oosl::common::operator_id operator_id_type;
			typedef oosl::common::operator_info operator_info_type;

			unary_expression(const operator_info_type &info, ptr_type operand);

			virtual ~unary_expression();

			virtual id_type id() override;

			virtual void echo(output_writer_type &writer) override;

		protected:
			virtual entry_type *evaluate_() override;

			operator_info_type info_;
			ptr_type operand_;
		};

		class binary_expression : public object{
		public:
			typedef oosl::common::operator_id operator_id_type;
			typedef oosl::common::operator_info operator_info_type;

			binary_expression(const operator_info_type &info, ptr_type left, ptr_type right);

			virtual ~binary_expression();

			virtual id_type id() override;

			virtual void echo(output_writer_type &writer) override;

		protected:
			virtual entry_type *evaluate_() override;

			operator_info_type info_;
			ptr_type left_;
			ptr_type right_;
		};
	}
}

#endif /* !OOSL_EXPRESSION_NODE_H */
