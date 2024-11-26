#ifndef SINTAKSIS_ANALYZER_H
#define SINTAKSIS_ANALYZER_H

#include "Token.h"
#include "TokenList.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip> // ��� std::setw

class SintaksisAnalyzer {
public:
    SintaksisAnalyzer();  // �����������
    ~SintaksisAnalyzer();
    //SintaksisAnalyzer(TokenList& tokenList); // ����������� ��������� ������ �� TokenList
    //void collectLine();              // ����� ��� ����� ������ �� tokenSequence
    void error(const int count_line, const std::string line, const std::string type_error);                    // ����� ��� ������ ������ 
    void building_tree(const int count_line, const std::string line);            // ����� ��� ���������� ������ �������
    std::string* split(const std::string line, int& numWords);

    bool isValidIdentifier(const std::string word); // ��������
    bool isValidExpression(const std::string expr); // ��������
    bool isValidOperator(const std::string opLine, const int count_line); // ��������
    bool isValidOperator(const std::string opLine); // ��������

    bool is_cycle(const std::string line, const int count_line); // ��������
    bool is_start_program(const std::string line, const int count_line); // �������� 
    bool is_end_program(const std::string line, const int count_line); // ��������
    bool is_descriptions(const std::string line, const int count_line); // ��������

    void draw_cycle(const std::string line); // ��������
    void draw_start_program(const std::string line); // ��������
    void draw_end_program(const std::string line); // ��������
    void draw_descriptions(const std::string line); // ��������
    void draw_operators(const std::string line); // ��������
    void draw_operators_cycle(const std::string line); // ��������
    void draw_varlist(const std::string line); // ��������
    void draw_expression(const std::string& expr, const std::string& indent);
    void clear_tree();
    void check_error();


    void trim(std::string& str) {
        // ������� ������� � ������ ������
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));

        // ������� ������� � ����� ������
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
    std::ofstream outputFile;  // ����� ��� ������ � ����
};

#endif
