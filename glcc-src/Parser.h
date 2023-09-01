#pragma once

#include "AST.h"
#include "Lexer.h"

class Parser
{
private:
    Lexer lex;
public:
    Parser(std::string buf);

    Node* Parse(std::string outPutFile);
};