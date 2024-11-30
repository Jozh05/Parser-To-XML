#pragma once
#include "Test_project/pch.h"
#include "Value.h"
#include <regex>

using std::cin, std::cout, std::endl;

std::unordered_map<std::string, Value> variables;

// Функция для обрезки пробелов с начала и конца строки
std::string GetLine() {

    std::string str;
    std::getline(std::cin, str);

    // Находим первый не-пробельный символ
    size_t start = str.find_first_not_of(" \t\r\n");
    // Если строка состоит только из пробелов, возвращаем пустую строку
    if (start == std::string::npos) return "";

    // Находим последний не-пробельный символ
    size_t end = str.find_last_not_of(" \t\r\n");

    // Возвращаем подстроку без пробелов
    return str.substr(start, end - start + 1);
}


std::vector<std::string> parseArray(const std::string& input) {
    std::vector<std::string> result;

    // Убираем внешние квадратные скобки
    std::string trimmedInput = input;
    if (trimmedInput.front() == '[' && trimmedInput.back() == ']') {
        trimmedInput = trimmedInput.substr(1, trimmedInput.length() - 2);
    }

    size_t start = 0;
    size_t length = trimmedInput.length();
    int braceCount = 0;  // Для отслеживания вложенности скобок
    bool insideQuote = false;  // Для отслеживания, находимся ли мы внутри кавычек

    // Разбиваем строку по запятым, игнорируя запятые внутри вложенных структур и строк
    for (size_t i = 0; i < length; ++i) {
        char ch = trimmedInput[i];

        // Если встречаем одиночную кавычку, меняем состояние (внутри кавычек или нет)
        if (ch == '\'' && (i == 0 || trimmedInput[i - 1] != '\\')) {
            insideQuote = !insideQuote;
        }

        // Увеличиваем/уменьшаем уровень вложенности при встрече скобок
        if (ch == '[' || ch == '(') {
            ++braceCount;
        }
        else if (ch == ']' || ch == ')') {
            --braceCount;
        }

        // Если текущий символ - запятая и мы не внутри кавычек и на уровне верхней вложенности
        if (ch == ',' && !insideQuote && braceCount == 0) {
            std::string element = trimmedInput.substr(start, i - start);
            // Убираем пробелы в начале и в конце
            element.erase(0, element.find_first_not_of(" \t"));
            element.erase(element.find_last_not_of(" \t") + 1);

            // Добавляем элемент в результат
            result.push_back(element);
            start = i + 1;  // Новый старт после запятой
        }
    }

    // Обрабатываем последний элемент (после последней запятой или конца строки)
    std::string lastElement = trimmedInput.substr(start);
    lastElement.erase(0, lastElement.find_first_not_of(" \t")); // Убираем пробелы слева
    lastElement.erase(lastElement.find_last_not_of(" \t") + 1); // Убираем пробелы справа

    result.push_back(lastElement); // Добавляем последний элемент

    return result;
}


std::vector<std::pair<std::string, std::string>> parseTable(const std::string& input) {
    
    if (!regex_match(input, (std::regex)R"(table\(([^()]*|table\([^\)]*\))+\))")) {
        std::cerr << "Syntax error. Invalid value";
        exit(-5);
    }
    
    std::vector<std::pair<std::string, std::string>> result;

    // Для поиска ключей и значений, включая строки, числа и вложенные структуры
    std::regex pattern(R"((\w+)\s*=>\s*(('[^']*')|(\d+)|(\[[^\[\]]*\])|(\!\{[a-zA-Z_]+\})|(table\([^\)]*\))))");
    std::smatch match;

    std::string::const_iterator searchStart(input.cbegin());

    while (regex_search(searchStart, input.cend(), match, pattern)) {
        std::string key = match[1];   // ключ
        std::string value = match[2];  // значение

        // Сохраняем пару ключ-значение
        result.push_back({ key, value });

        // Продолжаем с позиции после текущего совпадения
        searchStart = match.suffix().first;
    }

    return result;
}



Value parseVariable(std::string& element) {
    std::regex string_re("'[^']*'");
    std::regex int_re("^[+-]?\\d+$");
    std::regex arr_re(R"(\[[^\[\]]*(?:\[(?:[^\[\]]*(?:\[[^\[\]]*\])?[^\[\]]*)*\])?[^\[\]]*\])");
    std::regex empty_array_re(R"(\[\s*\])");
    std::regex table_re(R"(table\(([^()]*|table\([^\)]*\))+\))");
    std::regex var_re(R"((\!\{[a-zA-Z_]+\}))");

    if (regex_match(element, empty_array_re)) {
        std::vector<std::shared_ptr<Value>> result = {};
        return Value(result);
    }

    else if (regex_match(element, string_re)) {
        return Value(element.substr(1, element.length() - 2));
    }

    else if (regex_match(element, int_re)) {
        return Value(std::stoi(element));
    }

    else if (regex_match(element, arr_re)) {
        std::vector<std::shared_ptr<Value>> result;
        std::vector<std::string> elements = parseArray(element);
        for (auto& el : elements) {
            result.push_back(std::make_shared<Value>(parseVariable(el)));
        }
        return Value(result);
    }
    //TODO Распарсить словарь

    else if (regex_match(element, table_re)) {
        std::unordered_map<std::string, std::shared_ptr<Value>> result;
        std::vector<std::pair<std::string, std::string>> pairs = parseTable(element);
        for (auto& i : pairs) {
            result.emplace(i.first, std::make_shared<Value>(parseVariable(i.second)));
        }
        return Value(result);
    }

    else if (regex_match(element, var_re)) {
        std::string name = element.substr(2, element.length() - 3);
        return variables[name];
    }

    else {
        std::cerr << "Syntax error: invalid value" << endl;
        exit(-1);
    }
}