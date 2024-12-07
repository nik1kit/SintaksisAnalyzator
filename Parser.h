#ifndef PARSER_H
#define PARSER_H

#include "TreeNode.h"
#include <string>
#include <sstream>
#include <iostream>

class Parser {
public:
    static void parseProgram(TreeNode* root, const std::string& line, int& level) {
        std::istringstream lineStream(line);
        std::string word;

        TreeNode* currentNode = root;
        TreeNode* lastDescriptionNode = nullptr;

        while (lineStream >> word) {
            if (word == "PROGRAM") {
                currentNode->addSon(word, level, "WordsKey");
                std::string programName;
                if (lineStream >> programName) {
                    currentNode->addSon(programName, level, "Id");
                }
            }
            else if (word == "INTEGER") {
                if (!lastDescriptionNode) {
                    lastDescriptionNode = root->addSon("Descriptions", level);
                }
                TreeNode* descrNode = lastDescriptionNode->addSon("Descr", level + 1);
                descrNode->addSon("Type", level + 2)->addSon(word, level + 3, "WordsKey");

                std::string var;
                bool firstVar = true;  // ���� ��� ������ ����������
                while (lineStream >> var) {
                    if (var == ",") {
                        // ���� �������, ��������� � �������� � Symbols_of_Separating
                        TreeNode* SymbolsNode = descrNode->addSon("Varlist", level + 2);
                        SymbolsNode->addSon(var, level + 3, "Symbols_of_Separating");

                    }
                    else {
                        bool flag = false;
                        if (var.back() == ',') {
                            var = var.substr(0, var.size() - 1);
                            flag = true;
                        }
                        // ���� ��� �� �������, ��������� ����������
                        TreeNode* varNode = descrNode->addSon("Varlist", level + 2);
                        varNode->addSon(var, level + 3, "Id");
                        if (flag)
                            varNode->addSon(",", level + 3, "Symbols_of_Separating");
                        firstVar = false;  // ����� ������ ���������� ������� ����� �����������
                    }
                }
            }
            else if (word == "OPERATORS") {
                // ������� ����� ���� ��� ����� OPERATORS
                TreeNode* operatorsNode = currentNode->addSon("Operators", level);
                if (!operatorsNode) return; // ��������, ��� ���� ������

                std::string operatorWord;
                while (lineStream >> operatorWord) {
                    // ������ ����� ����� ��������� (����������, �������� "c")
                    std::string lhs = operatorWord; // ��������� ������ ���������� ��� ����� �������

                    // ������� ���� ��� ��������
                    TreeNode* opNode = operatorsNode->addSon("Op", level + 1);
                    if (!opNode) return;

                    // ����� ������� (����������, �������� "c")
                    TreeNode* lhsNode = opNode->addSon(lhs, level + 2, "Id");
                    if (!lhsNode) return;

                    // ������ �������� ������������ "="
                    lineStream >> operatorWord;  // ������ ������� ���� ������������ "="
                    opNode->addSon(operatorWord, level + 2, "Symbols_of_Operation"); // �������� "="

                    // ������ ��������� (��������, "a + ( b + 5 ) - x")
                    std::string token;
                    bool expectOperand = true;  // ������� ������� �������
                    bool inParentheses = false; // �������� �� ��������� ������

                    // ��� ������ ����� ������� ����� ���� ��� ���������
                    TreeNode* exprNode = opNode->addSon("Expr", level + 3);
                    if (!exprNode) return;

                    TreeNode* currentExprNode = nullptr;  // ���� ��� �������� SimpleExpr

                    while (lineStream >> token) {
                        if (token == "(") {
                            // ����������� ������ - �������� ��������� ���������
                            inParentheses = true;
                            exprNode->addSon("(", level + 6, "Opening_Bracket");
                            continue;
                        }

                        if (token == ")") {
                            // ����������� ������ - ����������� ��������� ���������
                            exprNode->addSon(")", level + 6, "Closing_Bracket");
                            inParentheses = false;
                            continue;
                        }

                        if (expectOperand) {
                            // ������� ������� (���������� ��� ���������)
                            if (!currentExprNode) {
                                // ���� ������� ������� �� ��� ������, ������� ����� SimpleExpr
                                currentExprNode = exprNode->addSon("SimpleExpr", level + 5);
                                if (!currentExprNode) return;
                            }

                            if (token[0] >= '0' && token[0] <= '9') {
                                // ���� ��� �����, �� ���������
                                currentExprNode->addSon(token, level + 6, "Const");
                            }
                            else {
                                // ���� ��� ����������
                                currentExprNode->addSon(token, level + 6, "Id");
                            }
                            expectOperand = false; // ��������� ������ ���� ��������
                        }
                        else {
                            // ������� ��������
                            exprNode->addSon(token, level + 6, "Symbols_of_Operation");
                            expectOperand = true; // ��������� ����� ������ ���� �������

                            // ����� �������� ������� ����� SimpleExpr ��� ���������� ��������
                            currentExprNode = exprNode->addSon("SimpleExpr", level + 5);
                            if (!currentExprNode) return;
                        }
                    }

                    // ����� ��������� ������������, ���� ��������� "END", ��������� ����
                    if (operatorWord == "END") {
                        operatorsNode->addSon(operatorWord, level + 2, "WordsKey");
                        break; // ����� �� ����� ����� ��������� ����� ����������
                    }
                }
            }
            else if (word == "FOR") {
                TreeNode* opNode = currentNode->addSon("Operators", level);
                if (!opNode) return; // ��������, ��� ���� ������
                opNode = opNode->addSon("Op", level + 1);
                if (!opNode) return; // ��������, ��� ���� ������
                opNode->addSon(word, level + 2, "WordsKey");

                // ������������ ��������� ����� FOR
                TreeNode* exprNode = opNode->addSon("Expr", level + 2);
                if (!exprNode) return; // ��������, ��� ���� ������

                std::string expr;
                TreeNode* simpleExprNode = nullptr;
                while (lineStream >> expr && expr != "TO") {
                    if (expr == "=") {
                        simpleExprNode = exprNode->addSon("SimpleExpr", level + 3);
                        if (!simpleExprNode) return;
                        simpleExprNode->addSon(expr, level + 4, "Symbols_of_Operation");
                    }
                    else if (expr == "+" || expr == "-" || expr == "*") {
                        if (!simpleExprNode) {
                            simpleExprNode = exprNode->addSon("SimpleExpr", level + 3);
                            if (!simpleExprNode) return;
                        }
                        simpleExprNode->addSon(expr, level + 4, "Symbols_of_Operation");
                    }
                    else {
                        if (!simpleExprNode) {
                            simpleExprNode = exprNode->addSon("SimpleExpr", level + 3);
                            if (!simpleExprNode) return;
                        }
                        simpleExprNode->addSon(expr, level + 4, expr[0] >= '0' && expr[0] <= '9' ? "Const" : "Id");
                    }
                }

                // ������������ ��������� ����� TO
                if (expr == "TO") {
                    opNode->addSon(expr, level + 2, "WordsKey");
                    TreeNode* toExprNode = opNode->addSon("Expr", level + 2);
                    if (!toExprNode) return;

                    simpleExprNode = nullptr;
                    while (lineStream >> expr && expr != "DO") {
                        if (expr == "(") {
                            toExprNode->addSon(expr, level + 3, "Opening_Bracket");
                        }
                        else if (expr == ")") {
                            toExprNode->addSon(expr, level + 3, "Closing_Bracket");
                        }
                        else if (expr == "+" || expr == "-" || expr == "=") {
                            simpleExprNode = toExprNode->addSon("SimpleExpr", level + 3);
                            if (!simpleExprNode) return;
                            simpleExprNode->addSon(expr, level + 4, "Symbols_of_Operation");
                        }
                        else {
                            if (!simpleExprNode) {
                                simpleExprNode = toExprNode->addSon("SimpleExpr", level + 3);
                                if (!simpleExprNode) return;
                            }
                            simpleExprNode->addSon(expr, level + 4, expr[0] >= '0' && expr[0] <= '9' ? "Const" : "Id");
                        }
                    }
                }

                // ���� ���������� DO, ������������ ���� �����
                if (expr == "DO") {
                    opNode->addSon(expr, level + 2, "WordsKey");

                    TreeNode* nestedCycleNode = opNode->addSon("NestedCycle", level + 2);
                    if (!nestedCycleNode) return;

                    TreeNode* nestedOpsNode = nestedCycleNode->addSon("Operators", level + 3);
                    if (!nestedOpsNode) return;

                    std::string nestedExpr;
                    while (lineStream >> nestedExpr) {
                        if (nestedExpr == "FOR") {
                            std::string nestedLine;
                            std::getline(lineStream, nestedLine);
                            nestedLine = "FOR " + nestedLine;
                            int nestedLevel = level + 4; // �������� ��������� �������
                            parseProgram(nestedOpsNode, nestedLine, nestedLevel);
                            break;
                        }
                        else {
                            // ��������� ��������� ���������
                            std::ostringstream expressionStream;
                            expressionStream << nestedExpr;

                            std::string remainingToken;
                            while (lineStream >> remainingToken && remainingToken != "FOR") {
                                expressionStream << " " << remainingToken;
                            }

                            // ���� ���������� "FOR", ����� ������� ��� ������� � �����
                            if (remainingToken == "FOR") {
                                lineStream.putback('F');
                                for (int i = 0; i < 3; ++i) lineStream.putback(remainingToken[i]); // FOR = 3 �������
                            }

                            // ���������� ��������� � ������� splitBySemicolonOrNewline
                            std::vector<std::string> expressions = splitBySemicolonOrNewline(expressionStream.str());
                            for (const auto& expr : expressions) {
                                // �������� Op ���� ��� ������� ���������
                                TreeNode* nestedOpNode = nestedOpsNode->addSon("Op", level + 4);
                                if (!nestedOpNode) return;

                                // ���������� ���� Expr ��� ���������
                                TreeNode* nestedExprNode = nestedOpNode->addSon("Expr", level + 5);
                                if (!nestedExprNode) return;

                                // ������ ���������
                                std::istringstream exprStream(expr);
                                std::string token;
                                TreeNode* simpleExprNode = nullptr;

                                bool expectOperand = true; // ������� ������� ������� (���������� ��� ���������)

                                while (exprStream >> token) {
                                    if (expectOperand) {
                                        // ������� ������� (���������� ��� ���������)
                                        simpleExprNode = nestedExprNode->addSon("SimpleExpr", level + 6);
                                        if (!simpleExprNode) return;

                                        simpleExprNode->addSon(token, level + 7, token[0] >= '0' && token[0] <= '9' ? "Const" : "Id");
                                        expectOperand = false; // ��������� ������ ���� ��������
                                    }
                                    else {
                                        // ������� ��������

                                        simpleExprNode->addSon(token, level + 7, "Symbols_of_Operation");
                                        expectOperand = true; // ��������� ����� ������ ���� �������
                                    }
                                }
                            }

                        }
                    }
                }
            }
            else if (word == "END") {
                currentNode->addSon(word, level, "WordsKey");
                std::string endName;
                if (lineStream >> endName) {
                    currentNode->addSon(endName, level, "Id");
                }
            }
            // ��������� ������ �������� ����...
        }
    }


    static std::vector<std::string> splitBySemicolonOrNewline(const std::string& line) {
        std::vector<std::string> expressions;
        std::vector<size_t> equal_positions;  // ������ ������� �������� '='

        // ������� ��� ������� �������� '=' � ������
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '=') {
                equal_positions.push_back(i);
            }
        }

        // ���� � ������ ������ ���� ���� '=', �� ���������� ��� ������ ��� ���� ���������
        if (equal_positions.size() == 1) {
            expressions.push_back(line);
            return expressions;
        }

        // ��������� ������ �� ��������� �� �������� '='
        size_t start_pos = 0;
        for (size_t i = 1; i < equal_positions.size(); ++i) {
            // ������ �� ����������� '=' �� �������� '='
            expressions.push_back(line.substr(start_pos, equal_positions[i] - start_pos - 2));
            start_pos = equal_positions[i] - 2;  // ����� ��������� ������� ����� '='
        }

        // ��������� ��������� ��������� (����� ���������� '=')
        expressions.push_back(line.substr(start_pos));

        return expressions;
    }
};

#endif // PARSER_H
