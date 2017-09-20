#include "common/interactive.h"

#include "test/test_runner.h"
#include "test/test_memory_manager.h"
#include "test/test_type_converter.h"
#include "test/test_drivers.h"

int main(){
	typedef oosl::common::interactive<std::string> interactive_type;
	interactive_type io;
	{//Hook callbacks and run
		io.hook<interactive_type::prompt_value_callback_type>(interactive_type::index_type::prompt_value, [](interactive_type::prompt_type type) -> std::string{
			switch (type){
			case interactive_type::prompt_type::introduction:
				return "Object Oriented Scripting Language\n----------------------------------\n";
			case interactive_type::prompt_type::input:
				return "Test Name: ";
			case interactive_type::prompt_type::unrecognized:
				return "Unrecognized Input\n";
			default:
				break;
			}

			return "";
		});

		io.hook<interactive_type::converter_callback_type>(interactive_type::index_type::converter, [](std::string value) -> std::string{
			for (auto &c : value)
				c = ::tolower(c);
			return value;
		});

		io.hook<interactive_type::action_callback_type>(interactive_type::index_type::action, [](const std::string &value) -> interactive_type::action_result_type{
			if (value == "quit")
				return interactive_type::action_result_type::quit;

			if (value == "ls"){//List tests
				std::string ls;
				for (auto &item : oosl_test_list)
					ls += (item + "\n");

				std::cout << ls;
			}
			else//Run test
				oosl::test::run(value.c_str());

			return interactive_type::action_result_type::nil;
		});

		io.run();
	}

	return 0;
}
