// HW_3_config.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <regex>
#include <stack>


using std::cin, std::cout, std::endl;

// Структура для хранения разных типов данных
struct Value {
    // Используем variant для хранения различных типов
    using VariantType = std::variant<int, std::string, std::vector<std::shared_ptr<Value>>, std::unordered_map<std::string, std::shared_ptr<Value>>>;

    VariantType data;

    // Конструкторы для разных типов данных
    Value(int val) : data(val) {}
    Value(const std::string& val) : data(val) {}
    Value(const std::vector<std::shared_ptr<Value>>& val) : data(val) {}
    Value(const std::unordered_map<std::string, std::shared_ptr<Value>>& val) : data(val) {}

    Value() = default;
    Value(const Value& value) : data(value.data) {}

    // Функция для вывода содержимого
    void print() const {
        std::visit([](const auto& val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
                std::cout << val << "(int)";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                std::cout << val << "(string)";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::vector<std::shared_ptr<Value>>>) {
                std::cout << "[ ";
                for (const auto& v : val) {
                    v->print();
                    std::cout << " ";
                }
                std::cout << "](array)";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::unordered_map<std::string, std::shared_ptr<Value>>>) {
                std::cout << "{ ";
                for (const auto& [key, v] : val) {
                    std::cout << key << ": ";
                    v->print();
                    std::cout << " ";
                }
                std::cout << "}";
            }
            }, data);
    }
};

// Функция для обрезки пробелов с начала и конца строки
std::string GetLine() {

    std::string str;
    std::getline(cin, str);

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

    // Разбиваем строку по запятым, игнорируя запятые внутри вложенных структур
    for (size_t i = 0; i < length; ++i) {
        char ch = trimmedInput[i];

        // Увеличиваем/уменьшаем уровень вложенности при встрече скобок
        if (ch == '[' || ch == '(') {
            ++braceCount;
        }
        else if (ch == ']' || ch == ')') {
            --braceCount;
        }

        // Если текущий символ - запятая и нет вложенных скобок, то это разделитель
        if (ch == ',' && braceCount == 0) {
            std::string element = trimmedInput.substr(start, i - start);
            // Убираем пробелы в начале и в конце
            element.erase(0, element.find_first_not_of(" \t"));
            element.erase(element.find_last_not_of(" \t") + 1);

            // Если элемент начинается с "!{" и заканчивается на "}", это специальный элемент
            if (element.size() > 2 && element.front() == '!' && element[1] == '{' && element.back() == '}') {
                result.push_back(element); // Добавляем элемент как есть
            }
            else {
                result.push_back(element); // Добавляем как обычный элемент
            }

            start = i + 1;  // Новый старт после запятой
        }
    }

    // Обрабатываем последний элемент (после последней запятой или конца строки)
    std::string lastElement = trimmedInput.substr(start);
    lastElement.erase(0, lastElement.find_first_not_of(" \t")); // Убираем пробелы слева
    lastElement.erase(lastElement.find_last_not_of(" \t") + 1); // Убираем пробелы справа

    // Если последний элемент начинается с "!{" и заканчивается на "}", это специальный элемент
    if (lastElement.size() > 2 && lastElement.front() == '!' && lastElement[1] == '{' && lastElement.back() == '}') {
        result.push_back(lastElement); // Добавляем элемент как есть
    }
    else {
        result.push_back(lastElement); // Добавляем как обычный элемент
    }

    for (const auto& i : result) {
        cout << i << endl;
    }

    return result;
}


std::vector<std::pair<std::string, std::string>> parseTable(const std::string& input) {
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

    for (const auto& i : result)
        cout << i.first << " : " << i.second << endl;

    return result;
}

std::unordered_map<std::string, Value> variables;

Value parseVariable(std::string& element) {
    std::regex string_re("'[^']*'");
    std::regex int_re("^[+-]?\\d+$");
    std::regex arr_re(R"(\[[^\[\]]*(?:\[(?:[^\[\]]*(?:\[[^\[\]]*\])?[^\[\]]*)*\])?[^\[\]]*\])");
    std::regex table_re(R"(table\(([^()]*|table\([^\)]*\))+\))");
    std::regex var_re(R"((\!\{[a-zA-Z_]+\}))");

    if (regex_match(element, string_re)) {
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
        return 1;
    }
}


int main()
{
    

    while (true) {
        //Строку пользовательского ввода
        std::string line(GetLine());

        if (line.find('\\') != std::string::npos) {
            line = line.substr(0, line.find('\\'));
        }

        if (line == "" || line == "\n")
            break;

        // Создаем регулярное выражение для разделения на лексемы
        std::regex re(R"(table\(([^()]*|table\([^\)]*\))+\)|\[[^\[\]]*(?:\[(?:[^\[\]]*(?:\[[^\[\]]*\])?[^\[\]]*)*\])?[^\[\]]*\]|'[^']*'|\S+)");
        std::sregex_token_iterator it(line.begin(), line.end(), re);
        std::sregex_token_iterator end;

        std::vector<std::string> words(it, end);

        for (const auto& i : words)
            cout << i << "\n";

        if (words.front() == "def" && words.size() == 4 && words[2] == ":=") {

            if (!std::regex_match(words[1], (std::regex)"[_a-zA-Z]+")) {
                std::cerr << "Incorrect variable name";
                return -1;
            }

            std::string value = words.back();
            std::string variable = words[1];

            variables.emplace(variable, parseVariable(value));
        }
        else {
            std::cerr << "Incorrect input";
            return -1;
        }
    }


    for (const auto& pair : variables) {
        cout << "Key: " << pair.first << " Value: "; 
        pair.second.print();
        cout << endl;
    }

}


/*// Создание вложенных словарей
    std::unordered_map<std::string, std::shared_ptr<Value>> innerDict = {
        {"innerKey1", std::make_shared<Value>(42)},
        {"innerKey2", std::make_shared<Value>("Inner Value")}
    };

    // Внешний словарь с вложенным словарем
    std::unordered_map<std::string, std::shared_ptr<Value>> outerDict = {
        {"outerKey1", std::make_shared<Value>(100)},
        {"outerKey2", std::make_shared<Value>("Outer Value")},
        {"outerKey3", std::make_shared<Value>(innerDict)}  // Вложенный словарь
    };

    // Создание объекта Value с внешним словарем
    Value dictVal(outerDict);

    // Вывод значений
    std::cout << "Dictionary with recursive structure: ";
    dictVal.print();
    std::cout << "\n";*/




// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
