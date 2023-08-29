#pragma once

#include "Token.h"

class Lexer
{
private:
    std::string buf;
    size_t cur_pos;
public:
    Lexer(std::string buf);

    bool GetNextToken(Token token);
};