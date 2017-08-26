#include "test/test_runner.h"
#include "test/test_memory_manager.h"
#include "test/test_type_converter.h"

int main(){
	auto result = oosl::test::run("type converter test");

	return result;
}
