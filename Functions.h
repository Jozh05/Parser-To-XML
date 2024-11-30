#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "Value.h"
#include <regex>

using std::cin, std::cout, std::endl;

extern std::unordered_map<std::string, Value> variables;

// Функция для обрезки пробелов с начала и конца строки
std::string GetLine();

std::vector<std::string> parseArray(const std::string& input);
std::vector<std::pair<std::string, std::string>> parseTable(const std::string& input);
Value parseVariable(std::string& element);


#endif