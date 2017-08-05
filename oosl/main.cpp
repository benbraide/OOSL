#include "test/test_runner.h"
#include "test/test_memory_manager.h"

int main(){
	auto result = oosl::test::run("memory manager test");

	return result;
}