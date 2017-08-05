#pragma once

#ifndef OOSL_TEST_RUNNER_H
#define OOSL_TEST_RUNNER_H

#include "test_include.h"

namespace oosl{
	namespace test{
		int run(const char *name){
			doctest::Context context;
			context.addFilter("test-case", name);

			auto result = context.run();
			context.clearFilters();

			return result;
		}
	}
}

#endif /* !OOSL_TEST_RUNNER_H */
