#include "LexicalAnalyzer.h"
#include "Token.h"
#include "SintaksisAnalyzer.h"
#include <iostream>

LexicalAnalyzer::LexicalAnalyzer(const std::string& inputFileName, const std::string& outputFileName) {
    inputFile.open(inputFileName);
    outputFile.open(outputFileName);
}


bool LexicalAnalyzer::isValidIdentifier(const std::string& word) const {
    // ���������, ������� �� ����� ������ �� ��������� � �������� ���� ���������� ��������
    for (char c : word) {
        if (!isalpha(c)) { // ���� ������ �� �������� ������
            return false; 
        }
    }
    return true; 
}


LexicalAnalyzer::~LexicalAnalyzer() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

TokenType LexicalAnalyzer::getKeywordOrIdentifier(const std::string& lexeme) {
    const char* keywords[] = { "INTEGER", "DO", "TO", "FOR"};
    TokenType keywordTokens[] = {
        TokenType::TYPE,
        TokenType::OP,
        TokenType::OP,
        TokenType::OP,
    };

    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        const char* keyword = keywords[i];
        int len = strlen(keyword);

        // �������� �� �����: ���� ����� �� ���������, ��� �� �������� �����
        if (lexeme.length() != len) {
            continue;
        }

        // ��������� �����������
        bool match = true;
        for (int j = 0; j < len; ++j) {
            if (lexeme[j] != keyword[j]) {
                match = false;
                break;
            }
        }

        // ���� �������� ����� �������
        if (match) {
            return keywordTokens[i];
        }
    }

    // ���� �� ������� �������� �����, ��� �������������
    return TokenType::ID_NAME;
}


TokenType LexicalAnalyzer::getNumber(const std::string& lexeme) {
    // ���������, �������� �� ������ ����� ������
    for (char c : lexeme) {
        if (!isdigit(c)) {
            return TokenType::ERROR; 
        }
    }
    return TokenType::INT_NUM;
}

bool LexicalAnalyzer::isDelimiter(char c) const {
    return (c == '(' || c == ')' || c == ',' || c == ';' || c == '\'' || c == '\"' || c == '.');
}

void LexicalAnalyzer::handleDelimiters(char c) {
    std::string delim(1, c);
    tokenList.addToken(Token(TokenType::DELIMITER, delim, -1));
}

Token LexicalAnalyzer::getNextLexeme() {
    std::string word;
    char c;
    int index = 1;
    bool lastWasOperator = false;
    while (inputFile.get(c)) {
        if (c == '\n' || c == '\r' || c =='\n\r') {
            tokenList.addToken(Token(TokenType::END_LINE, "END_LINE", index));
            count_line++;
            if (!lexeme.empty() && lexeme != "\n") {
                sintaksis_analyzer.trim(lexeme);
                sintaksis_analyzer.building_tree(count_line, lexeme);
            }
            lexeme = "";
        }
        if (isspace(c)) {
            index++;
            lexeme += " ";
            continue;
        }
        if (isalpha(c)) { // ������������� ��� �������� �����
            word.clear();
            if (c == 'P') {  // ��������� ������ ����� "PROGRAM"
                std::string programWord = "PROGRAM";
                bool isProgram = true;
                std::string potential_error;

                for (size_t i = 1; i < programWord.length(); i++) {
                    potential_error += c;
                    if (!inputFile.get(c) || c != programWord[i]) {
                        isProgram = false;
                        break;
                    }
                }

                if (isProgram) {  // ���� ����� "PROGRAM" ���� �������
                    inputFile.get(c);  // ��������� ��������� ������ ����� ����� "PROGRAM"
                    if (isspace(c) || c == ':' || c == EOF) {  // ��������� �� ������, ����������� ��� ����� �����
                        std::string id;
                        if (inputFile >> id && isValidIdentifier(id)) {
                            tokenList.addToken(Token(TokenType::PROGRAM, "PROGRAM", index));
                            Token nextToken = Token(TokenType::BEGIN, id, index + 1);
                            lexeme += "PROGRAM " + id;
                            return nextToken;
                        }
                        else {
                            lexeme += id;
                            return Token(TokenType::ERROR, id, index);  // ������, ���� ������������� ����������
                        }
                    }
                    else {
                        std::string errorWord = "PROGRAM" + std::string(1, c);  // ��������� ������ ������������ ������

                        // �������� ���������� ������������ ������� �� ������� ��� ����� ������
                        while (inputFile.get(c) && !isspace(c) && c != EOF) {
                            errorWord += c;
                        }
                        lexeme += errorWord;
                        return Token(TokenType::ERROR, errorWord, index);  // ���������� ������ ������������ ����� ��� ������
                    }

                }
                else {
                    potential_error += c;
                    // ��������� ���������� ������������ �������
                    while (inputFile.get(c) && !isspace(c) && c != EOF) {
                        potential_error += c;
                    }
                    if (isValidIdentifier(potential_error)) {
                        lexeme += potential_error;
                        return Token(TokenType::ID_NAME, potential_error, index);
                    }
                    lexeme += potential_error;
                    return Token(TokenType::ERROR, potential_error, index);  // ���������� ������ ������������ ����� ��� ������
                }
            }

            if (c == 'E') {  // ��������� ������ ����������� "END"
                std::string endWord = "END";
                bool isEnd = true;
                std::string potential_error;

                // ��������� ��������� ����� "END"
                for (size_t i = 1; i < endWord.length(); i++) {
                    potential_error += c;
                    if (!inputFile.get(c) || c != endWord[i]) {
                        isEnd = false;
                        break;
                    }
                }

                if (isEnd) {  // ���� ����� "END" ���� �������
                    inputFile.get(c);  // ��������� ��������� ������
                    if (isspace(c) || c == ':' || c == EOF) {  // ��������� �� ������, ��������� ��� ����� �����
                        tokenList.addToken(Token(TokenType::END, "END", index));  // ��������� ����� END
                        lexeme += "END ";
                        return Token(TokenType::END, "END", index);  // ���������� ����� END
                    }
                    else {
                        std::string errorWord = "END " + std::string(1, c);  // ��������� ������ ������������ ������

                        // �������� ���������� ������������ ������� �� ������� ��� ����� ������
                        while (inputFile.get(c) && !isspace(c) && c != EOF) {
                            errorWord += c;
                        }

                        lexeme += errorWord;
                        return Token(TokenType::ERROR, errorWord, index);  // ���������� ������ ������������ ����� ��� ������
                    }
                }
                else {
                    potential_error += c;
                    // ��������� ���������� ������������ �������
                    while (inputFile.get(c) && !isspace(c) && c != EOF) {
                        potential_error += c;
                    }

                    if (isValidIdentifier(potential_error)) {
                        lexeme += potential_error;
                        return Token(TokenType::ID_NAME, potential_error, index);
                    }

                    lexeme += potential_error;
                    return Token(TokenType::ERROR, potential_error, index);  // ���������� ������ ������������ ����� ��� ������
                }
            }


            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // ���������� ��������� ������ � �����



            // �������� �� �������������
            if (isValidIdentifier(word)) {
                TokenType type = getKeywordOrIdentifier(word);
                lexeme += word;
                return Token(type, word, index);
            }
            else {
                lexeme += word;
                return Token(TokenType::ERROR, word, index);
            }
        }
        else if (isdigit(c)) { // ��������� ������ �����
            word.clear();
            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // ���������� ��������� ������ � �����
            for (int j = 0; j < word.size(); j++) {
                if (!isdigit(word[j])) {
                    lexeme += word;
                    return Token(TokenType::ERROR, word, index);
                    break;
                }
            }
            lexeme += word;
            return Token(TokenType::INT_NUM, word, index);

        }



        else if (c == '=' || c == '+' || c == '-') {  // ���������
            if (lastWasOperator) {
                return Token(TokenType::ERROR, "Consecutive operators", index);  // ���� ��� ��������� ������, ���������� ������
            }
            lastWasOperator = true;  // ������������� ����, ��� ��������� ������ ��� ����������
            word = c;

            if (inputFile.peek() == c && (c == '+' || c == '-')) {  // ��������� �� ++ ��� --
                inputFile.get();  // ��������� ������ ��������
                word += c;
                lexeme += word;
                return Token(TokenType::ERROR, word, index);  // ���������� ������ ��� ++ ��� --
            }
            lexeme += word;
            return Token(TokenType::OPERATOR, word, index);  // ���������� ��������
        }

        else if (isDelimiter(c)) { // �����������
            lexeme += std::string(1, c);
            return Token(TokenType::DELIMITER, std::string(1, c), index);
        }

        else {
            // ���� ���������� ��������� ������, �������� �������� ��������� �������
            word.clear();
            word += c;
            while (inputFile.get(c) && !isspace(c) && !isDelimiter(c)) {
                word += c;
            }
            inputFile.unget(); // ������� ��������� ���������� ������
            lexeme += word;
            return Token(TokenType::ERROR, word, index);
        }
    }

    return Token(TokenType::UNKNOWN, "", index); // UNKNOWN ����� � ����� �����
}




void LexicalAnalyzer::analyze() {
    Token token;
    while ((token = getNextLexeme()).type != TokenType::UNKNOWN) {

        // ��������� ����� � ������
        tokenList.addToken(token);

    }


    tokenList.printTokens(outputFile);

    sintaksis_analyzer.check_error();
    sintaksis_analyzer.Printing_Tree();
    //sintaksis_analyzer.Printing_Specific_Tree("Operators", 3);
}

