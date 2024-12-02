// HW_3_config.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "Value.h"
#include "Functions.h"


int main(int argc, char* argv[])
{
    
    if (argc != 2) {
        std::cerr << "Using: HW_3_config <path to xml file>";
        return 1;
    }

    while (true) {
        //Строку пользовательского ввода
        std::string line(GetLine());

        if (line.find('\\') != std::string::npos) {
            line = line.substr(0, line.find('\\'));
            if (line == "")
                continue;
        }

        if (line == "" || line == "\n")
            break;

        // Создаем регулярное выражение для разделения на лексемы
        std::regex re(R"(table\(([^()]*|table\([^\)]*\))+\)|\[[^\[\]]*(?:\[(?:[^\[\]]*(?:\[[^\[\]]*\])?[^\[\]]*)*\])?[^\[\]]*\]|'[^']*'|\S+)");
        std::sregex_token_iterator it(line.begin(), line.end(), re);
        std::sregex_token_iterator end;

        std::vector<std::string> words(it, end);

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
       pair.second.check();
       cout << "Key: " << pair.first << " Value: "; 
       pair.second.print();
       cout << endl;
    }

    tinyxml2::XMLDocument doc;
    /*
    tinyxml2::XMLElement* rootElement = doc.NewElement("data");
    doc.InsertEndChild(rootElement);
    */

    for (const auto& [varName, value] : variables) {
        tinyxml2::XMLElement* element = doc.NewElement(varName.c_str());
        doc.InsertEndChild(element);
        value.toXml(element, doc);
    }

//    doc.SaveFile(argv[1]);
    doc.SaveFile(argv[1]);

}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
