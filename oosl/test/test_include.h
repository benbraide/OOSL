#pragma once

#ifndef OOSL_TEST_INCLUDE_H
#define OOSL_TEST_INCLUDE_H

#define DOCTEST_CONFIG_IMPLEMENT

#include <list>
#include <string>

#include "../external/doctest/doctest.h"

std::list<std::string> oosl_test_list;

#define OOSL_ADD_TEST(class_name, test_name)			\
namespace oosl{											\
	namespace test{										\
		class class_name{								\
		public:											\
			class_name(){								\
				oosl_test_list.push_back(test_name);	\
			}											\
		} class_name ## _instance;						\
	}													\
}

#endif /* !OOSL_TEST_INCLUDE_H */
