#pragma once

#ifndef OOSL_MEMORY_DEPENDENCY_H
#define OOSL_MEMORY_DEPENDENCY_H

namespace oosl{
	namespace memory{
		class dependency{
		public:
			virtual ~dependency() = default;
		};

		template <class value_type>
		class value_dependency : public dependency{
		public:
			typedef value_type value_type;

			explicit value_dependency(const value_type &value)
				: value_(value){}

			virtual ~value_dependency() = default;

			value_type &value(){
				return value_;
			}

		protected:
			value_type value_;
		};
	}
}

#endif /* !OOSL_MEMORY_DEPENDENCY_H */
