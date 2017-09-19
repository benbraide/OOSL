#pragma once

#ifndef OOSL_INTERACTIVE_H
#define OOSL_INTERACTIVE_H

#include <any>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "string_converter.h"

namespace oosl{
	namespace common{
		template <class value_type>
		class interactive{
		public:
			typedef value_type value_type;

			enum class index_type{
				prompt,
				prompt_value,
				input,
				input_value,
				converter,
				action,
			};

			enum class prompt_type{
				introduction,
				input,
				information,
				unrecognized,
			};

			enum class action_result_type{
				nil,
				quit,
				unrecognized,
			};

			typedef std::function<void(prompt_type)> prompt_callback_type;
			typedef std::function<std::string(prompt_type)> prompt_value_callback_type;

			typedef std::function<void(std::string &)> input_callback_type;
			typedef std::function<value_type()> value_input_callback_type;
			typedef std::function<value_type(std::string)> converter_callback_type;

			typedef std::function<action_result_type(const value_type &)> action_callback_type;
			typedef std::unordered_map<index_type, std::any> callback_list_type;

			template <typename callback_type>
			void hook(index_type index, callback_type callback){
				callback_list_[index] = callback;
			}

			void run(){
				auto action_callback = find_action_callback_();
				auto prompt_callback = find_prompt_callback_();

				prompt_callback(prompt_type::introduction);
				while (true){//Run loop
					prompt_callback(prompt_type::input);
					switch (action_callback((find_value_input_callback_())())){
					case action_result_type::unrecognized:
						prompt_callback(prompt_type::unrecognized);
						break;
					case action_result_type::quit:
						return;
					default:
						break;
					}
				}
			}

		private:
			prompt_callback_type find_prompt_callback_(){
				auto entry = callback_list_.find(index_type::prompt);
				if (entry != callback_list_.end())//Entry found
					return std::any_cast<prompt_callback_type>(entry->second);

				return [this](prompt_type type){
					std::cout << (find_prompt_value_callback_())(type);
				};
			}

			prompt_value_callback_type find_prompt_value_callback_(){
				auto entry = callback_list_.find(index_type::prompt_value);
				if (entry != callback_list_.end())//Entry found
					return std::any_cast<prompt_value_callback_type>(entry->second);

				return [this](prompt_type type) -> std::string{
					switch (type){
					case prompt_type::introduction:
						return "Interactive Introduction";
					case prompt_type::input:
						return "Enter Value: ";
					case prompt_type::information:
						return "Interactive Information";
					case prompt_type::unrecognized:
						return "Unrecognized Input";
					default:
						break;
					}

					return "";
				};
			}

			input_callback_type find_input_callback_(){
				auto entry = callback_list_.find(index_type::input);
				if (entry != callback_list_.end())//Entry found
					return std::any_cast<input_callback_type>(entry->second);

				return [this](std::string &buffer){
					std::getline(std::cin, buffer);
				};
			}

			value_input_callback_type find_value_input_callback_(){
				auto entry = callback_list_.find(index_type::input_value);
				if (entry != callback_list_.end())//Entry found
					return std::any_cast<value_input_callback_type>(entry->second);

				return [this]() -> value_type{
					std::string input;
					(find_input_callback_())(input);
					return (find_converter_callback_())(input);
				};
			}

			converter_callback_type find_converter_callback_(){
				auto entry = callback_list_.find(index_type::converter);
				if (entry != callback_list_.end())//Entry found
					return std::any_cast<converter_callback_type>(entry->second);

				return [this](const std::string &buffer) -> value_type{
					return string_converter<value_type>::convert(buffer);
				};
			}

			action_callback_type find_action_callback_(){
				return std::any_cast<action_callback_type>(callback_list_[index_type::action]);
			}

			callback_list_type callback_list_;
		};
	}
}

#endif /* !OOSL_INTERACTIVE_H */
