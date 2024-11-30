#include "Test_project/pch.h"
#include "Value.h"

void Value::print() const {
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

void Value::check() const {
    std::visit([](const auto& val) {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            if (val.length() == 0) {
                std::cerr << "Empty strings are not supported";
                exit(-2);
            }
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::vector<std::shared_ptr<Value>>>) {
            if (val.empty()) {
                std::cerr << "Empty arrays are not supported";
                exit(-3);
            }
            for (const auto& v : val) 
                v->check();
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::unordered_map<std::string, std::shared_ptr<Value>>>) {
            if (val.empty()) {
                std::cerr << "Empty tables are not supported";
                exit(-4);
            }
            for (const auto& [key, v] : val)
                v->check();
        }
        }, data);
}

void Value::toXml(tinyxml2::XMLElement* parentElement, tinyxml2::XMLDocument& doc) const {
    std::visit([&doc, &parentElement](const auto& val) {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
            // Если значение int, добавляем его как текст в тег с именем переменной
            parentElement->SetText(val);
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            // Если значение string, добавляем его как текст в тег с именем переменной
            parentElement->SetText(val.c_str());
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::vector<std::shared_ptr<Value>>>) {
            // Если значение — массив, добавляем элементы как дочерние элементы
            for (size_t i = 0; i < val.size(); ++i) {
                tinyxml2::XMLElement* itemElement = doc.NewElement("item");
                parentElement->InsertEndChild(itemElement);
                val[i]->toXml(itemElement, doc); // Рекурсивно обрабатываем каждый элемент массива
            }
        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::unordered_map<std::string, std::shared_ptr<Value>>>) {
            // Если значение — словарь, добавляем пары ключ-значение как дочерние элементы
            for (const auto& [key, v] : val) {
                tinyxml2::XMLElement* keyElement = doc.NewElement(key.c_str());
                parentElement->InsertEndChild(keyElement);
                v->toXml(keyElement, doc); // Рекурсивно обрабатываем значение
            }
        }
        }, data);
}