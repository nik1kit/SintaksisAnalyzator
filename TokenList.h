#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "Token.h"
#include <fstream>

class TokenList {
public:
    TokenList();
    ~TokenList();
    static const int MAX_TOKENS = 10000; // ������������ ���������� �������
    Token* tokenSequence[MAX_TOKENS]; // ��� ������ ������� �� �������
    void addToken(const Token& token);
    int hashFunction(const std::string& lexeme) const;

    void printTokens(std::ofstream& outputFile);

private:
    static const int HASH_TABLE_SIZE = 1000;
    Token* hashTable[HASH_TABLE_SIZE];

    int tokenCount; // ������� ���������� ����������� �������
};

#endif 