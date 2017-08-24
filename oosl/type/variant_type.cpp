#include "variant_type.h"

oosl::type::variant::variant(ptr_list_type &&underlying_types)
	: underlying_types_(underlying_types){}

oosl::type::variant::~variant() = default;

oosl::type::object::driver_type *oosl::type::variant::driver(){
	return nullptr;//#TODO: Implement
}

oosl::type::object *oosl::type::variant::underlying_type(){
	return reinterpret_cast<object *>(&underlying_types_);
}

std::string oosl::type::variant::name(){
	std::string list;
	for (auto underlying_type : underlying_types_){
		if (list.empty())
			list = underlying_type->name();
		else//Append
			list += (", " + underlying_type->name());
	}

	return ("variant_t<" + list + ">");
}

std::string oosl::type::variant::print(){
	std::string list;
	for (auto underlying_type : underlying_types_){
		if (list.empty())
			list = underlying_type->print();
		else//Append
			list += (", " + underlying_type->print());
	}

	return ("variant_t<" + list + ">");
}

oosl::type::object::size_type oosl::type::variant::size(){
	return static_cast<size_type>(sizeof(uint64_type));
}

int oosl::type::variant::score(object &type){
	auto lowest_score = OOSL_MAX_TYPE_SCORE;
	for (auto underlying_type : underlying_types_){
		auto score = underlying_type->score(type);
		if (score != OOSL_MIN_TYPE_SCORE && score < lowest_score)
			lowest_score = score;
		else if (score == OOSL_MAX_TYPE_SCORE)
			return score;
	}

	return lowest_score;
}

oosl::type::object *oosl::type::variant::match(object &type, match_type criteria){
	for (auto underlying_type : underlying_types_){
		auto matched = underlying_type->match(type, criteria);
		if (matched != nullptr)
			return matched;
	}

	return nullptr;
}

bool oosl::type::variant::is_variant(){
	return true;
}
