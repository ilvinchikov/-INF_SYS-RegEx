#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <ctime>
#include <unordered_set>
#include "UsefulFunctions.h"

// refs:
/**
 * https://thk.kz/index.php/informatsiya/stati/320-regulyarnye-vyrazheniya
 * https://habr.com/ru/companies/otus/articles/532056/
 * https://habr.com/ru/articles/545150/
 */

using namespace std;

class RegexProject {
private:
    UsefulFunctions us;
public:
    RegexProject() {
        display();
    }

    // монитор выбора задания
    void display(){
        string text;
        for (;;) {
            cout << "==========================\nЗадача: \n1. Проверка корректности пароля (с regex) -> pass.txt \n2. Проверка корректности WEB-цвета (с regex) -> colors.txt \n3. Токенизация мат.выражения -> tokens.txt \n4. Проверка корректности дат -> dates.txt \n5. task1_part2 -> skobki.txt \n6. task2_part2 -> text.txt \nДля выхода оставьте поле пустым\n==========================\n\nВыбор: ";
            getline(cin, text);
            cout << "[INFO] ВВОД: '" << text << "'" << endl;
            if (us.trim(text) == "" || us.trim(text).length() == 0) break;
            if (text.find("1") != string::npos) mainTask(1);
            else if (text.find("2") != string::npos) mainTask(2);
            else if (text.find("3") != string::npos) mainTask(3);
            else if (text.find("4") != string::npos) mainTask(4);
            else if (text.find("5") != string::npos) mainTask(5);
            else if (text.find("6") != string::npos) mainTask(6);
            else cout << "Error. Вбейте число от 1 до 6" << endl;
        }
    }

    // запуск основного процесса
    void mainTask(int numTask) {
        string path;
        cout << "file path: ";
        getline(cin, path);
        ifstream file(path);
        if (!file.is_open()) {
            cout << "Ошибка: не удалось открыть файл.\n" << endl;
            return;
        }
        try {
            bool checker = false;
            string text;
            while (getline(file, text)) {
                checker = true;
                switch (numTask) {
                case 1:
                    passChecker(text);
                    break;
                case 2:
                    webColorChecker(text);
                    break;
                case 3:
                    tokenization(text);
                    break;
                case 4:
                    dateChecker(text);
                    break;
                case 5:
                    task5(text);
                    break;
                case 6:
                    task6(text);
                    break;
                default:
                    break;
                }
            }
            if (!checker) cout << "Ошибка: файл пуст.\n" << endl;
            file.close();
        }
        catch(const exception& e) {
            cerr << e.what() << endl;
            file.close();
        }
    }

    // [с регулярными выражениями] - pC
    void passChecker(string text) {
        if (pC_validCheck(text)) cout << "[1.1] УСПЕШНО: " << text << endl;
        else cout << "[1.1] ПРОВАЛ: " << text << endl;
    }

    // проверка на вывод
    bool pC_validCheck(const string& text) {
        // 8 символов
        if (text.length() < 8) return false;

        // одинаковые символы
        for(auto i=0; i<text.length()-1; i++) {
            if (text[i] == text[i+1]) return false;
        }
        // допустимые символы
        regex r_chars = regex("^[A-Za-z0-9\^$%@#&*!?]+$");
        if (!regex_match(text, r_chars)) return false;

        // одна заглавная буква
        regex r_upper = regex(".*[A-Z].*");
        if (!regex_search(text, r_upper)) return false;

        // одна строчная буква
        regex r_lower = regex(".*[a-z].*");
        if (!regex_search(text, r_lower)) return false;

        // одна цифра
        regex r_digit = regex(".*[0-9].*");
        if (!regex_search(text, r_digit)) return false;

        // два различных спец символа
        regex r_spec = regex("[^$%@#&*!?]");                // указываем, какие символы не надо стирать
        string specChars = regex_replace(text, r_spec, ""); // стираем все, кроме наших выбранных
        unordered_set<char> uniqueSpecChars(specChars.begin(), specChars.end()); // заносим символы, повторы не вносятся
        if (uniqueSpecChars.size() < 2) return false;

        // проверка пройдена
        return true;
    }


    // [с регулярными выражениями] - wCC
    void webColorChecker(string text) {
        // Проверка HEX формата (#rgb или #rrggbb)
        regex r_hex("^#([0-9a-fA-F]{3}|[0-9a-fA-F]{6})$");
        if (regex_match(text, r_hex)) {
            // проверка пройдена
            cout << "[1.2-hex] УСПЕШНО: " << text << endl;
            return;
        }

        // проверка RGB формата (rgb(r,g,b) или rgb(r%,g%,b%))
        regex r_rgb(R"(^rgb\(\s*(\d{1,3}%?|\d{1,3}%?)\s*,\s*(\d{1,3}%?|\d{1,3}%?)\s*,\s*(\d{1,3}%?|\d{1,3}%?)\s*\)$)");
        smatch rgbMatch;
        if (regex_match(text, rgbMatch, r_rgb)) {
            for (auto i=1; i <= 3; i++) {
                string value = rgbMatch[i].str();
                if (value.back() == '%') {
                    // процентное значение (от 0% до 100%)
                    int percent = us.toInteger(value.substr(0, value.size() - 1));
                    if (percent < 0 || percent > 100) {
                        cout << "[1.2-rgb(1)] ПРОВАЛ: " << text << endl;
                        return;
                    }
                } 
                else {
                    // числовое значение (от 0 до 255)
                    int num = us.toInteger(value);
                    if (num < 0 || num > 255) {
                        cout << "[1.2-rgb(2)] ПРОВАЛ: " << text << endl;
                        return;
                    }
                }
            }
            // проверка пройдена
            cout << "[1.2-rgb] УСПЕХ: " << text << endl;
            return;
        }

        // проверка HSL формата (hsl(h,s%,l%))
        regex r_hsl(R"(^hsl\(\s*(\d{1,3})\s*,\s*(\d{1,3}%)\s*,\s*(\d{1,3}%)\s*\)$)");
        smatch hslMatch;
        if (regex_match(text, hslMatch, r_hsl)) {
            // проверка тона (от 0 до 360)
            int hue = us.toInteger(hslMatch[1].str());
            if (hue < 0 || hue > 360) {
                cout << "[1.2-hsl(1)] ПРОВАЛ: " << text << endl;
                return;
            }

            // проверка насыщенности и светлоты (от 0% до 100%)
            for (int i = 2; i <= 3; i++) {
                string value = hslMatch[i].str();
                int percent = us.toInteger(value.substr(0, value.size() - 1));
                if (percent < 0 || percent > 100) {
                    cout << "[1.2-hsl(2)] ПРОВАЛ: " << text << endl;
                    return;
                }
            }
            // проверка пройдена
            cout << "[1.2-hsl] УСПЕХ: " << text << endl;
            return;
        }

        // проверка провалена: соответствия по regex не найдены
        cout << "[1.2] ПРОВАЛ: " << text << endl;
        return;
    }

    // [без регулярок] - tknz
    void tokenization(string text) {

        // структура токена
        struct Token {
            string type;                // тип токена: переменная, число, константа, функция, операция, скобки
            pair<size_t, size_t> span;  // пространство, занимаемое место в выражении
        };
        vector<Token> tokens;
        size_t pos = 0;
        size_t len = text.length();

        // предопределенные множества констант и функций
        unordered_set<string> constants = {"pi", "e", "sqrt2", "ln2", "ln10"};
        unordered_set<string> functions = {"sin", "cos", "tg", "ctg", "tan", "cot",
             "sinh", "cosh", "th", "cth", "tanh", "coth", "ln", "lg", "log", "exp", 
             "sqrt", "cbrt", "abs", "sign" };
        unordered_set<char> operators = {'^', '*', '/', '-', '+'};

        while (pos < len) {
            // пропускаем пробелы
            if (isspace(text[pos])) {
                pos++;
                continue;
            }

            // проверяем скобки
            if (text[pos] == '(') {
                tokens.push_back({"left_parenthesis", {pos, pos + 1}});
                pos++;
                continue;
            }
            if (text[pos] == ')') {
                tokens.push_back({"right_parenthesis", {pos, pos + 1}});
                pos++;
                continue;
            }

            // проверяем операторы
            if (operators.count(text[pos])) {
                tokens.push_back({"operator", {pos, pos + 1}});
                pos++;
                continue;
            }

            // проверяем числа
            if (isdigit(text[pos]) || (text[pos] == '.' && pos+1 < len && isdigit(text[pos + 1]))) {
                size_t start = pos;
                bool has_dot = false;
                while (pos<len && (isdigit(text[pos]) || (text[pos] == '.' && !has_dot))) {
                    if (text[pos] == '.') has_dot = true;
                    pos++;
                }
                tokens.push_back({"number", {start, pos}});
                continue;
            }

            // проверяем идентификаторы (переменные, функции, константы)
            if (isalpha(text[pos]) || text[pos] == '_') {
                size_t start = pos;
                while (pos < len && (isalnum(text[pos]) || text[pos] == '_')) {
                    pos++;
                }
                string ident = text.substr(start, pos - start);

                // проверяем константы
                if (constants.count(ident)) {
                    tokens.push_back({"constant", {start, pos}});
                    continue;
                }

                // проверяем функции
                if (functions.count(ident)) {
                    tokens.push_back({"function", {start, pos}});
                    continue;
                }

                // если не константа и не функция, то это переменная
                tokens.push_back({"variable", {start, pos}});
                continue;
            }

            // если встретили неизвестный символ, пропускаем его
            pos++;
        }

        if (tokens.size() > 0) {
            for (auto i=0; i<tokens.size(); i++) {
                cout << tokens[i].type << " [" << tokens[i].span.first << ", " << tokens[i].span.second << ")\n";
            }
        }
        else {
            cout << "[1.3] ПРОВАЛ (токены не найдены):" << text << endl;
        }
    }

    // [без регулярок] - dC
    void dateChecker(string text) {
        // Месяцы на английском и русском
        map<string, int> months = {
            {"января", 1}, {"февраля", 2}, {"марта", 3}, {"апреля", 4},
            {"мая", 5}, {"июня", 6}, {"июля", 7}, {"августа", 8},
            {"сентября", 9}, {"октября", 10}, {"ноября", 11}, {"декабря", 12},
            {"january", 1}, {"february", 2}, {"march", 3}, {"april", 4},
            {"may", 5}, {"june", 6}, {"july", 7}, {"august", 8},
            {"september", 9}, {"october", 10}, {"november", 11}, {"december", 12},
            {"jan", 1}, {"feb", 2}, {"mar", 3}, {"apr", 4}, {"jun", 6},
            {"jul", 7}, {"aug", 8}, {"sep", 9}, {"oct", 10}, {"nov", 11}, {"dec", 12}      
        };

        // конвертируем в новую строку с только пробелами
        string s = text;
        for (auto i=0; i<s.size(); i++) {
            if (s[i] == '.' || s[i] == '-' || s[i] == '/' || s[i] == ',') {
                s[i] = ' '; // заменяем разделители на пробелы
            }
        }

        // разбиваем целую строку на части и заносим их в вектор
        istringstream iss(s);
        vector<string> elements;
        string elem;
        while (iss >> elem) {
            elements.push_back(elem);
        }

        int d = -1, m = -1, y = -1;

        for (auto i=0; i<elements.size(); i++) {
            string t = elements[i];
            string t_low = us.toLowerCase(t);
            
            if (months.count(t_low)) {
                m = months.at(t_low);
            } 
            else {
                istringstream numStream(t);
                int num = 0;
                if (numStream >> num) {
                    if (num > 31) y = num;
                    else if (d == -1) d = num;
                    else if (m == -1) m = num;
                    else if (y == -1) y = num;
                } 
                else {
                    cout << "[1.4] ПРОВАЛ-1: " << text << endl;
                    return;
                }
            }
        }

        if (d < 1 || m < 1 || y < 1 || y > 9999) {
            cout << "[1.4] ПРОВАЛ-2: " << text << endl;
            return;
        }

        if (d > dC_daysInMonth(y, m)) {
            cout << "[1.4] ПРОВАЛ-3: " << text << endl;
            return;
        }


        // успех
        cout << "[1.4] УСПЕХ: " << text << endl;
        return;
    }


    // проверка на високосный год
    bool dC_isVisokosniy (int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    };

    // возврат кол-ва дней в месяце (идёт учет високосного года)
    int dC_daysInMonth(int y, int m) {
        if (m < 1 || m > 12) return 0;  // базовая проверка допустимости номера месяца

        int days[] { 31,28,31,30,31,30,31,31,30,31,30,31 }; // массив дней по месяцам

        if (m == 2 && dC_isVisokosniy(y)) return 29;    // февраль високосного года (29 дней)
        else return days[m - 1];
    };

    // 2.1 Проверка корректности скобочного выражения
    void task5(string text) {
        string test = text;
        // проверяем на посторонние символы
        regex allowed = regex("^[\\{\\}\\[\\]\\(\\)]*$");
        if (!regex_match(test, allowed)) {
            cout << "[2.1] ПРОВАЛ: " << text << endl;
            return;
        }

        regex pairPattern = regex("\\(\\)|\\[\\]|\\{\\}");
        // удаляем пары скобок до тех пор, пока они есть
        while (true) {
            string new_str = regex_replace(test, pairPattern, "");
            if (new_str == test) {
                break;
            }
            test = new_str;
        }

        // если всё удалилось — скобки корректны
        if (test.empty()) {
            cout << "[2.1] УСПЕХ: " << text << endl;
            return;
        }
        else {
            cout << "[2.1] ПРОВАЛ-2: " << text << endl;
            return;
        }
    }

    // 2.2. Разбиение текста на предложения
    void task6(string text) {
        // базовая проверка на пустоту
        if (text.empty()) {
            cout << "[2.2] ПРОВАЛ (ТЕКСТ-пуст): " << text << endl;
            return;
        }

        // аббревиатуры
        vector<string> abbreviations = {
            "и т.п.", "т.е.", "и.др.", "ул.", "до н.э.", "рис.", "т.п.", "см.",
            "табл.", "т.к.", "т.д.", "т.ч.", "и пр.", "т.н.", "и.т.п.", "т.о.",
            "д.", "г.", "и.т.д.", "напр.", "н.э.", "прим.", "т.д."
        };

        vector<string> sentences;
        string current;
        
        for (size_t i = 0; i < text.size(); i++) {
            current += text[i];

            // если обнаруживаем предполагаемый конец предложения
            if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
                bool isAbbreviation = false;
                
                // проверяем все возможные аббревиатуры
                for (const auto& abbr : abbreviations) {
                    // проверяем, заканчивается ли текущая строка на аббревиатуру
                    if (current.size() >= abbr.size() && 
                        current.substr(current.size() - abbr.size()) == abbr) {
                        isAbbreviation = true;
                        break;
                    }
                }
                
                // если это не аббревиатура и следующий символ - пробел или конец строки
                if (!isAbbreviation && (i + 1 >= text.size() || isspace(text[i+1]) || text[i+1] == '\"' || text[i+1] == '\'')) {
                    sentences.push_back(current);
                    current.clear();
                }
            }
        }

        if (!current.empty()) {
            sentences.push_back(current);
        }

        cout << "\n[2.2] РЕЗУЛЬТАТ:\n";
        for (size_t i = 0; i < sentences.size(); i++) {
            cout << "[" << task6_getRandomColor(i) << "] " << sentences[i] << endl;
        }
    }

    // возврат рандомного цвета (в формате строки)
    string task6_getRandomColor(int digit) {
        vector<string> colors = {
            "red", "green", "blue", "yellow", "purple", "teal", "orange", "gray",
            "silver", "gold", "cyan", "lime", "olive", "aqua", "pink", "brown"
        };
        srand(time(0));
        int index = (rand() + digit) % colors.size();
        return colors[index];
    }
    
};
