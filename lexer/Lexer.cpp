#include "Lexer.h"

Lexer::Lexer(std::string buf)
{
    this->buf = buf;
    buf.append("\0");
    cur_pos = 0;
}

bool IsPunctuation(char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}';
}

bool Lexer::GetNextToken(Token token)
{
    while (buf[cur_pos])
    {
        // Skip whitespace and invalid chars
_skip_space:
        while (buf[cur_pos] <= 32 && buf[cur_pos])
        {
            cur_pos++;
            goto _skip_space;
        }
        if (buf[cur_pos] == '/' && buf[cur_pos+1] == '/')
        {
            while (buf[cur_pos] != '\n')
                cur_pos++;
            goto _skip_space;
        }
        if (!buf[cur_pos])
            return false;
        
        if (buf[cur_pos] == '"')
        {
            printf("String\n");
            exit(1);
        }
        else if (buf[cur_pos] >= '0' && buf[cur_pos] <= '9')
        {
            printf("Number\n");
            exit(1);
        }
        else
        {
            std::string tok;
            while (buf[cur_pos] && buf[cur_pos] > 32 && !IsPunctuation(buf[cur_pos]))
                tok += buf[cur_pos++];
            printf("%s\n", tok.c_str());
            token.type = Types::IDENTIFIER;
            token.val = tok;
            return true;
        }

        return true;
    }

    return false;
}
