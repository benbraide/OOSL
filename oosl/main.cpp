#include "common/interactive.h"

#include "test/test_runner.h"
#include "test/test_memory_manager.h"
#include "test/test_type_converter.h"
#include "test/test_drivers.h"

#include "lexer/literal_grammar.h"

int main(){
	oosl::lexer::skipper skipper;
	oosl::lexer::literal_grammar gram;

	std::vector<OOSL_AST_QNAME(lit)> ast_list;
	std::vector<oosl::node::object::ptr_type> node_list;

	std::string buffer;
	std::getline(std::cin, buffer);

	auto pv = boost::spirit::qi::phrase_parse(buffer.c_str(), buffer.c_str() + buffer.size(), gram >> *gram, skipper, ast_list);
	if (pv){
		for (auto &ast : ast_list)//Convert to node
			node_list.push_back(oosl::lexer::apply_visitor<OOSL_AST_JOIN(OOSL_AST_QNAME(lit), _visitor)>(ast.lit));

		for (auto nod : node_list)
			std::cout << nod->print() << std::endl;
	}

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
