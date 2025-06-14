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

    // ������� ������ �������
    void display(){
        string text;
        for (;;) {
            cout << "==========================\n������: \n1. �������� ������������ ������ (� regex) -> pass.txt \n2. �������� ������������ WEB-����� (� regex) -> colors.txt \n3. ����������� ���.��������� -> tokens.txt \n4. �������� ������������ ��� -> dates.txt \n5. task1_part2 -> skobki.txt \n6. task2_part2 -> text.txt \n��� ������ �������� ���� ������\n==========================\n\n�����: ";
            getline(cin, text);
            cout << "[INFO] ����: '" << text << "'" << endl;
            if (us.trim(text) == "" || us.trim(text).length() == 0) break;
            if (text.find("1") != string::npos) mainTask(1);
            else if (text.find("2") != string::npos) mainTask(2);
            else if (text.find("3") != string::npos) mainTask(3);
            else if (text.find("4") != string::npos) mainTask(4);
            else if (text.find("5") != string::npos) mainTask(5);
            else if (text.find("6") != string::npos) mainTask(6);
            else cout << "Error. ������ ����� �� 1 �� 6" << endl;
        }
    }

    // ������ ��������� ��������
    void mainTask(int numTask) {
        string path;
        cout << "file path: ";
        getline(cin, path);
        ifstream file(path);
        if (!file.is_open()) {
            cout << "������: �� ������� ������� ����.\n" << endl;
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
            if (!checker) cout << "������: ���� ����.\n" << endl;
            file.close();
        }
        catch(const exception& e) {
            cerr << e.what() << endl;
            file.close();
        }
    }

    // [� ����������� �����������] - pC
    void passChecker(string text) {
        if (pC_validCheck(text)) cout << "[1.1] �������: " << text << endl;
        else cout << "[1.1] ������: " << text << endl;
    }

    // �������� �� �����
    bool pC_validCheck(const string& text) {
        // 8 ��������
        if (text.length() < 8) return false;

        // ���������� �������
        for(auto i=0; i<text.length()-1; i++) {
            if (text[i] == text[i+1]) return false;
        }
        // ���������� �������
        regex r_chars = regex("^[A-Za-z0-9\^$%@#&*!?]+$");
        if (!regex_match(text, r_chars)) return false;

        // ���� ��������� �����
        regex r_upper = regex(".*[A-Z].*");
        if (!regex_search(text, r_upper)) return false;

        // ���� �������� �����
        regex r_lower = regex(".*[a-z].*");
        if (!regex_search(text, r_lower)) return false;

        // ���� �����
        regex r_digit = regex(".*[0-9].*");
        if (!regex_search(text, r_digit)) return false;

        // ��� ��������� ���� �������
        regex r_spec = regex("[^$%@#&*!?]");                // ���������, ����� ������� �� ���� �������
        string specChars = regex_replace(text, r_spec, ""); // ������� ���, ����� ����� ���������
        unordered_set<char> uniqueSpecChars(specChars.begin(), specChars.end()); // ������� �������, ������� �� ��������
        if (uniqueSpecChars.size() < 2) return false;

        // �������� ��������
        return true;
    }


    // [� ����������� �����������] - wCC
    void webColorChecker(string text) {
        // �������� HEX ������� (#rgb ��� #rrggbb)
        regex r_hex("^#([0-9a-fA-F]{3}|[0-9a-fA-F]{6})$");
        if (regex_match(text, r_hex)) {
            // �������� ��������
            cout << "[1.2-hex] �������: " << text << endl;
            return;
        }

        // �������� RGB ������� (rgb(r,g,b) ��� rgb(r%,g%,b%))
        regex r_rgb(R"(^rgb\(\s*(\d{1,3}%?|\d{1,3}%?)\s*,\s*(\d{1,3}%?|\d{1,3}%?)\s*,\s*(\d{1,3}%?|\d{1,3}%?)\s*\)$)");
        smatch rgbMatch;
        if (regex_match(text, rgbMatch, r_rgb)) {
            for (auto i=1; i <= 3; i++) {
                string value = rgbMatch[i].str();
                if (value.back() == '%') {
                    // ���������� �������� (�� 0% �� 100%)
                    int percent = us.toInteger(value.substr(0, value.size() - 1));
                    if (percent < 0 || percent > 100) {
                        cout << "[1.2-rgb(1)] ������: " << text << endl;
                        return;
                    }
                } 
                else {
                    // �������� �������� (�� 0 �� 255)
                    int num = us.toInteger(value);
                    if (num < 0 || num > 255) {
                        cout << "[1.2-rgb(2)] ������: " << text << endl;
                        return;
                    }
                }
            }
            // �������� ��������
            cout << "[1.2-rgb] �����: " << text << endl;
            return;
        }

        // �������� HSL ������� (hsl(h,s%,l%))
        regex r_hsl(R"(^hsl\(\s*(\d{1,3})\s*,\s*(\d{1,3}%)\s*,\s*(\d{1,3}%)\s*\)$)");
        smatch hslMatch;
        if (regex_match(text, hslMatch, r_hsl)) {
            // �������� ���� (�� 0 �� 360)
            int hue = us.toInteger(hslMatch[1].str());
            if (hue < 0 || hue > 360) {
                cout << "[1.2-hsl(1)] ������: " << text << endl;
                return;
            }

            // �������� ������������ � �������� (�� 0% �� 100%)
            for (int i = 2; i <= 3; i++) {
                string value = hslMatch[i].str();
                int percent = us.toInteger(value.substr(0, value.size() - 1));
                if (percent < 0 || percent > 100) {
                    cout << "[1.2-hsl(2)] ������: " << text << endl;
                    return;
                }
            }
            // �������� ��������
            cout << "[1.2-hsl] �����: " << text << endl;
            return;
        }

        // �������� ���������: ������������ �� regex �� �������
        cout << "[1.2] ������: " << text << endl;
        return;
    }

    // [��� ���������] - tknz
    void tokenization(string text) {

        // ��������� ������
        struct Token {
            string type;                // ��� ������: ����������, �����, ���������, �������, ��������, ������
            pair<size_t, size_t> span;  // ������������, ���������� ����� � ���������
        };
        vector<Token> tokens;
        size_t pos = 0;
        size_t len = text.length();

        // ���������������� ��������� �������� � �������
        unordered_set<string> constants = {"pi", "e", "sqrt2", "ln2", "ln10"};
        unordered_set<string> functions = {"sin", "cos", "tg", "ctg", "tan", "cot",
             "sinh", "cosh", "th", "cth", "tanh", "coth", "ln", "lg", "log", "exp", 
             "sqrt", "cbrt", "abs", "sign" };
        unordered_set<char> operators = {'^', '*', '/', '-', '+'};

        while (pos < len) {
            // ���������� �������
            if (isspace(text[pos])) {
                pos++;
                continue;
            }

            // ��������� ������
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

            // ��������� ���������
            if (operators.count(text[pos])) {
                tokens.push_back({"operator", {pos, pos + 1}});
                pos++;
                continue;
            }

            // ��������� �����
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

            // ��������� �������������� (����������, �������, ���������)
            if (isalpha(text[pos]) || text[pos] == '_') {
                size_t start = pos;
                while (pos < len && (isalnum(text[pos]) || text[pos] == '_')) {
                    pos++;
                }
                string ident = text.substr(start, pos - start);

                // ��������� ���������
                if (constants.count(ident)) {
                    tokens.push_back({"constant", {start, pos}});
                    continue;
                }

                // ��������� �������
                if (functions.count(ident)) {
                    tokens.push_back({"function", {start, pos}});
                    continue;
                }

                // ���� �� ��������� � �� �������, �� ��� ����������
                tokens.push_back({"variable", {start, pos}});
                continue;
            }

            // ���� ��������� ����������� ������, ���������� ���
            pos++;
        }

        if (tokens.size() > 0) {
            for (auto i=0; i<tokens.size(); i++) {
                cout << tokens[i].type << " [" << tokens[i].span.first << ", " << tokens[i].span.second << ")\n";
            }
        }
        else {
            cout << "[1.3] ������ (������ �� �������):" << text << endl;
        }
    }

    // [��� ���������] - dC
    void dateChecker(string text) {
        // ������ �� ���������� � �������
        map<string, int> months = {
            {"������", 1}, {"�������", 2}, {"�����", 3}, {"������", 4},
            {"���", 5}, {"����", 6}, {"����", 7}, {"�������", 8},
            {"��������", 9}, {"�������", 10}, {"������", 11}, {"�������", 12},
            {"january", 1}, {"february", 2}, {"march", 3}, {"april", 4},
            {"may", 5}, {"june", 6}, {"july", 7}, {"august", 8},
            {"september", 9}, {"october", 10}, {"november", 11}, {"december", 12},
            {"jan", 1}, {"feb", 2}, {"mar", 3}, {"apr", 4}, {"jun", 6},
            {"jul", 7}, {"aug", 8}, {"sep", 9}, {"oct", 10}, {"nov", 11}, {"dec", 12}      
        };

        // ������������ � ����� ������ � ������ ���������
        string s = text;
        for (auto i=0; i<s.size(); i++) {
            if (s[i] == '.' || s[i] == '-' || s[i] == '/' || s[i] == ',') {
                s[i] = ' '; // �������� ����������� �� �������
            }
        }

        // ��������� ����� ������ �� ����� � ������� �� � ������
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
                    cout << "[1.4] ������-1: " << text << endl;
                    return;
                }
            }
        }

        if (d < 1 || m < 1 || y < 1 || y > 9999) {
            cout << "[1.4] ������-2: " << text << endl;
            return;
        }

        if (d > dC_daysInMonth(y, m)) {
            cout << "[1.4] ������-3: " << text << endl;
            return;
        }


        // �����
        cout << "[1.4] �����: " << text << endl;
        return;
    }


    // �������� �� ���������� ���
    bool dC_isVisokosniy (int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    };

    // ������� ���-�� ���� � ������ (��� ���� ����������� ����)
    int dC_daysInMonth(int y, int m) {
        if (m < 1 || m > 12) return 0;  // ������� �������� ������������ ������ ������

        int days[] { 31,28,31,30,31,30,31,31,30,31,30,31 }; // ������ ���� �� �������

        if (m == 2 && dC_isVisokosniy(y)) return 29;    // ������� ����������� ���� (29 ����)
        else return days[m - 1];
    };

    // 2.1 �������� ������������ ���������� ���������
    void task5(string text) {
        string test = text;
        // ��������� �� ����������� �������
        regex allowed = regex("^[\\{\\}\\[\\]\\(\\)]*$");
        if (!regex_match(test, allowed)) {
            cout << "[2.1] ������: " << text << endl;
            return;
        }

        regex pairPattern = regex("\\(\\)|\\[\\]|\\{\\}");
        // ������� ���� ������ �� ��� ���, ���� ��� ����
        while (true) {
            string new_str = regex_replace(test, pairPattern, "");
            if (new_str == test) {
                break;
            }
            test = new_str;
        }

        // ���� �� ��������� � ������ ���������
        if (test.empty()) {
            cout << "[2.1] �����: " << text << endl;
            return;
        }
        else {
            cout << "[2.1] ������-2: " << text << endl;
            return;
        }
    }

    // 2.2. ��������� ������ �� �����������
    void task6(string text) {
        // ������� �������� �� �������
        if (text.empty()) {
            cout << "[2.2] ������ (�����-����): " << text << endl;
            return;
        }

        // ������������
        vector<string> abbreviations = {
            "� �.�.", "�.�.", "�.��.", "��.", "�� �.�.", "���.", "�.�.", "��.",
            "����.", "�.�.", "�.�.", "�.�.", "� ��.", "�.�.", "�.�.�.", "�.�.",
            "�.", "�.", "�.�.�.", "����.", "�.�.", "����.", "�.�."
        };

        vector<string> sentences;
        string current;
        
        for (size_t i = 0; i < text.size(); i++) {
            current += text[i];

            // ���� ������������ �������������� ����� �����������
            if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
                bool isAbbreviation = false;
                
                // ��������� ��� ��������� ������������
                for (const auto& abbr : abbreviations) {
                    // ���������, ������������� �� ������� ������ �� ������������
                    if (current.size() >= abbr.size() && 
                        current.substr(current.size() - abbr.size()) == abbr) {
                        isAbbreviation = true;
                        break;
                    }
                }
                
                // ���� ��� �� ������������ � ��������� ������ - ������ ��� ����� ������
                if (!isAbbreviation && (i + 1 >= text.size() || isspace(text[i+1]) || text[i+1] == '\"' || text[i+1] == '\'')) {
                    sentences.push_back(current);
                    current.clear();
                }
            }
        }

        if (!current.empty()) {
            sentences.push_back(current);
        }

        cout << "\n[2.2] ���������:\n";
        for (size_t i = 0; i < sentences.size(); i++) {
            cout << "[" << task6_getRandomColor(i) << "] " << sentences[i] << endl;
        }
    }

    // ������� ���������� ����� (� ������� ������)
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
