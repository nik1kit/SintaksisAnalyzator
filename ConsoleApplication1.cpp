#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "SintaksisAnalyzer.h"
#include "Token.h"
#include "TokenList.h"

int main() {
    LexicalAnalyzer lexer("input.txt", "output.txt");
    lexer.analyze();
    //TokenList& tokenList = lexer.getTokenList();
    //SintaksisAnalyzer sintaksis(tokenList);
    //sintaksis.collectLine();
    return 0;
}