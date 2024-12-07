#ifndef SINTAKSIS_ANALYZER_H
#define SINTAKSIS_ANALYZER_H

#include "Token.h"
#include "TokenList.h"
#include "TreeNode.h"
#include "Parser.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
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
    bool isValidOperator_for_cylce(const std::string opLine); // ��������

    bool is_cycle(const std::string line, const int count_line); // ��������
    bool is_start_program(const std::string line, const int count_line); // �������� 
    bool is_end_program(const std::string line, const int count_line); // ��������
    bool is_descriptions(const std::string line, const int count_line); // ��������

    
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

    void Printing_Tree() {
        std::ofstream outFile("parsing_tree.txt");

        // �������� �� �������� �������� �����
        if (outFile.is_open()) {
            // ������ ������ � ����
            int maxLevel = root->getMaxLevel(root);
            root->printTree(outFile, maxLevel);  // rootNode � ��� ������ ������ ������
            outFile.close();  // ��������� ���� ����� ������
        }
        else {
            std::cerr << "������ ��� �������� �����!" << std::endl;
        }
    }

    void Printing_Specific_Tree(const std::string& Nodename, int indentation = 0) {
        std::ofstream outFile("parsing_tree.txt");

        // �������� �� �������� �������� �����
        if (outFile.is_open()) {
            // ������ ������ � ����
            root->printSpecificNode(outFile, Nodename, indentation);  // rootNode � ��� ������ ������ ������
            outFile.close();  // ��������� ���� ����� ������
        }
        else {
            std::cerr << "������ ��� �������� �����!" << std::endl;
        }
    }

    std::vector<std::string> splitBySemicolonOrNewline(const std::string& str);


private:
    TreeNode* root = new TreeNode("Program", 0);
    int level = 1;
    std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i != 0) result += delimiter;
            result += parts[i];
        }
        return result;
    }

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
