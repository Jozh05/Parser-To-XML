#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stack>

#include<tinyxml2.h>

struct Value {
	
	using VariantType = std::variant<int, std::string, std::vector<std::shared_ptr<Value>>, std::unordered_map<std::string, std::shared_ptr<Value>>>;
	
	VariantType data;
	
	Value(int val) : data(val) {}
	Value(const std::string& val) : data(val) {}
	Value(const std::vector<std::shared_ptr<Value>>& val) : data(val) {}
	Value(const std::unordered_map<std::string, std::shared_ptr<Value>>& val) : data(val) {}

	Value() = default;
	Value(const Value& value) : data(value.data) {}

	void print() const;

	void toXml(tinyxml2::XMLElement* parentElement, tinyxml2::XMLDocument& doc) const;

	void check() const;
};