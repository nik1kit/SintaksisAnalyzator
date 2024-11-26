#ifndef SINTAKSIS_ANALYZER_H
#define SINTAKSIS_ANALYZER_H

#include "Token.h"
#include "TokenList.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip> // Для std::setw

class SintaksisAnalyzer {
public:
    SintaksisAnalyzer();  // Конструктор
    ~SintaksisAnalyzer();
    //SintaksisAnalyzer(TokenList& tokenList); // Конструктор принимает ссылку на TokenList
    //void collectLine();              // Метод для сбора строки из tokenSequence
    void error(const int count_line, const std::string line, const std::string type_error);                    // Метод для вывода ошибок 
    void building_tree(const int count_line, const std::string line);            // Метод для построения дерева разбора
    std::string* split(const std::string line, int& numWords);

    bool isValidIdentifier(const std::string word); // работает
    bool isValidExpression(const std::string expr); // работает
    bool isValidOperator(const std::string opLine, const int count_line); // работает
    bool isValidOperator(const std::string opLine); // работает

    bool is_cycle(const std::string line, const int count_line); // работает
    bool is_start_program(const std::string line, const int count_line); // работает 
    bool is_end_program(const std::string line, const int count_line); // работает
    bool is_descriptions(const std::string line, const int count_line); // работает

    void draw_cycle(const std::string line); // работает
    void draw_start_program(const std::string line); // работает
    void draw_end_program(const std::string line); // работает
    void draw_descriptions(const std::string line); // работает
    void draw_operators(const std::string line); // работает
    void draw_operators_cycle(const std::string line); // работает
    void draw_varlist(const std::string line); // работает
    void draw_expression(const std::string& expr, const std::string& indent);
    void clear_tree();
    void check_error();


    void trim(std::string& str) {
        // Удаляем пробелы с начала строки
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));

        // Удаляем пробелы с конца строки
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), str.end());
    }

private:
    std::string types_lexeme[1000];
    int count_types_lexeme = 0;
    bool is_valid_start = false;
    bool is_valid_end = false;
    bool is_valid_descriptions = false;
    bool is_valid_operator = false;
    bool is_error_flag = false;
    std::ofstream outputFile;  // Поток для записи в файл
};

#endif
