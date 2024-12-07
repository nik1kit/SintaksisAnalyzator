#include "TokenList.h"
#include <iostream>
#include <iomanip> // ��� std::setw

TokenList::TokenList() : tokenCount(0) {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable[i] = nullptr;
    }
    for (int i = 0; i < MAX_TOKENS; ++i) {
        tokenSequence[i] = nullptr;
    }
}

TokenList::~TokenList() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        delete hashTable[i];
    }
    for (int i = 0; i < MAX_TOKENS; ++i) {
        delete tokenSequence[i];
    }
}

int TokenList::hashFunction(const std::string& lexeme) const {
    int hash = 0;
    for (char c : lexeme) {
        hash = (hash * 31 + c) % HASH_TABLE_SIZE; // ������������ ��� ��������
    }
    return hash;
}


void TokenList::addToken(const Token& token) {
    // ��������� � ���-�������
    int index = hashFunction(token.lexeme);
    while (hashTable[index] != nullptr &&
        (hashTable[index]->lexeme != token.lexeme ||
            hashTable[index]->type != token.type)) {
        index = (index + 1) % HASH_TABLE_SIZE; // ������������ ��� ��������
    }

    if (tokenCount < MAX_TOKENS) {
        tokenSequence[tokenCount] = new Token(token.type, token.lexeme, index);
        tokenCount++;
    }

    // ���� ������ �����, ��������� ����� �����
    if (hashTable[index] == nullptr) {
        hashTable[index] = new Token(token.type, token.lexeme, index);
    }
}


void TokenList::printTokens(std::ofstream& outputFile) {
    bool printed[MAX_TOKENS] = { false }; // ������ ��� ������������ ���������� �������
    for (int i = 0; i < MAX_TOKENS; ++i) {
        Token* token = tokenSequence[i];
        if (token != nullptr) {
            //std::cout << tokenSequence[i]->lexeme << "\n";
            // ���������, ��� �� ����� ��� �������
            bool isUnique = true;
            for (int j = 0; j < i; ++j) {
                if (tokenSequence[j] != nullptr &&
                    tokenSequence[j]->lexeme == token->lexeme &&
                    tokenSequence[j]->type == token->type) {
                    isUnique = false;
                    break;
                }
            }
            if (isUnique) {
                std::string tokenType;
                switch (token->type) {
                case TokenType::PROGRAM:
                    tokenType = "PROGRAM";
                    break;
                case TokenType::OP:
                    tokenType = "OP";
                    break;
                case TokenType::BEGIN:
                    tokenType = "BEGIN";
                    break;
                case TokenType::END:
                    tokenType = "END";
                    break;
                case TokenType::DESCRIPTIONS:
                    tokenType = "DESCRIPTIONS";
                    break;
                case TokenType::TYPE:
                    tokenType = "TYPE";
                    break;
                case TokenType::ID_NAME:
                    tokenType = "ID_NAME";
                    break;
                case TokenType::INT_NUM:
                    tokenType = "INT_NUM";
                    break;
                case TokenType::OPERATOR:
                    tokenType = "OPERATOR";
                    break;
                case TokenType::DELIMITER:
                    tokenType = "DELIMITER";
                    break;
                case TokenType::ERROR:
                    tokenType = "ERROR";
                    continue;  // ���������� ���������� ������ � �������
                default:
                    tokenType = "UNKNOWN";
                    break;
                }
                
                outputFile << std::setw(15) << std::left << tokenType   
                    << " | "
                    << std::setw(20) << std::left << token->lexeme 
                    << " | "
                    << token->index << std::endl;
            }
        }
    }
    outputFile << "\n";
    for (int i = 0; i < MAX_TOKENS; ++i) {
        Token* token = tokenSequence[i];
        if (tokenSequence[i] != nullptr && token->type == TokenType::ERROR) {
            bool isUnique = true;
            for (int j = 0; j < i; ++j) {
                if (tokenSequence[j] != nullptr &&
                    tokenSequence[j]->lexeme == token->lexeme &&
                    tokenSequence[j]->type == token->type) {
                    isUnique = false;
                }
            }
            if (isUnique) {
                outputFile << std::setw(15) << std::left << "ERROR"
                    << " | "
                    << std::setw(20) << std::left << token->lexeme
                    << " | "
                    << token->index << std::endl;
            }
        }
    }
}


