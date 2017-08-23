#include "function_type.h"

#include "../driver/driver_object.h"
#include "../storage/storage_entry.h"

oosl::type::function_type::function_type(ptr_type return_type, ptr_list_type &&parameters)
	: dynamic(id_type::function_), return_type_(return_type), parameters_(std::move(parameters)){}

oosl::type::function_type::~function_type() = default;

oosl::type::object &oosl::type::function_type::function_return_type(){
	return *return_type_;
}

oosl::type::object::ptr_list_type &oosl::type::function_type::function_parameters(){
	return parameters_;
}

std::string oosl::type::function_type::name(){
	std::string parameters;
	for (auto parameter : parameters_){
		if (parameters.empty())
			parameters = parameter->name();
		else//Append
			parameters += (", " + parameter->name());
	}

	return ("function_t<" + return_type_->name() + "(" + parameters + ")>");
}

std::string oosl::type::function_type::print(){
	std::string parameters;
	for (auto parameter : parameters_){
		if (parameters.empty())
			parameters = parameter->print();
		else//Append
			parameters += (", " + parameter->print());
	}

	return ("function_t<" + return_type_->print() + "(" + parameters + ")>");
}

int oosl::type::function_type::score(object &type){
	auto value = object::score(type);
	if (value != OOSL_MIN_TYPE_SCORE)
		return value;

	if (!type.is_strong_function())//Type is not a strong function
		return (type.is_function() ? (OOSL_MAX_TYPE_SCORE - 1) : OOSL_MIN_TYPE_SCORE);

	auto &type_parameters = type.function_parameters();
	if (parameters_.size() != type_parameters.size())
		return OOSL_MIN_TYPE_SCORE;//Parameter count mismatch

	auto score = return_type_->score(type.function_return_type());
	if (score < (OOSL_MAX_TYPE_SCORE - 2))
		return OOSL_MIN_TYPE_SCORE;//Return type mismatch

	auto lowest_score = score;
	for (auto left = parameters_.begin(), right = type_parameters.begin(); left != parameters_.end(); ++left, ++right){
		if ((score = (*left)->score(**right)) < (OOSL_MAX_TYPE_SCORE - 2))
			return OOSL_MIN_TYPE_SCORE;//Parameter mismatch

		if (score < lowest_score)//Update lowest score
			lowest_score = score;
	}

	return lowest_score;
}

int oosl::type::function_type::score(const storage_entry_list_type &entries){
	if (parameters_.empty())//Empty parameter list
		return (entries.empty() ? OOSL_MAX_TYPE_SCORE : OOSL_MIN_TYPE_SCORE);

	auto lowest_score = OOSL_MAX_TYPE_SCORE, score = 0;
	auto entry = entries.begin();
	
	object *variadic_type = nullptr;
	for (auto parameter = parameters_.begin(); parameter != parameters_.end() && entry != entries.end(); ++entry){
		if (variadic_type != nullptr || (*parameter)->is_variadic()){//Variadic parameter
			variadic_type = (*parameter)->non_variadic();
			if ((score = variadic_type->score(*(*entry)->type->driver()->type(**entry)->non_modified())) < (OOSL_MAX_TYPE_SCORE - 2))
				return OOSL_MIN_TYPE_SCORE;//Parameter mismatch
		}
		else{//Non-variadic
			if ((score = (*parameter)->score(*(*entry)->type->driver()->type(**entry)->non_modified())) < (OOSL_MAX_TYPE_SCORE - 2))
				return OOSL_MIN_TYPE_SCORE;//Parameter mismatch
			++parameter;//Advance iterator
		}

		if (score < lowest_score)//Update lowest score
			lowest_score = score;
	}

	return lowest_score;
}

bool oosl::type::function_type::is_function(){
	return true;
}

bool oosl::type::function_type::is_strong_function(){
	return true;
}
