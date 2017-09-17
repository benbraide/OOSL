#include <iostream>

#include "test/test_runner.h"
#include "test/test_memory_manager.h"
#include "test/test_type_converter.h"

int main(){
	std::string val;

	std::cout << "Run Test?[y/n]: ";
	std::getline(std::cin, val);

	if (val == "y" || val == "Y"){
		std::cout << "Test Name: ";
		std::getline(std::cin, val);
		return oosl::test::run(val.c_str());
	}
	
	return 0;
}
