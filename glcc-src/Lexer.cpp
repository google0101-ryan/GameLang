#include "Lexer.h"

Lexer::Lexer(std::string buf)
{
    this->buf = buf;
    buf.append("\0");
    cur_pos = 0;
}

bool IsPunctuation(char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == '@' || c == ',';
}

bool IsOperation(char c)
{
    return c == '+' || c == '-' || c == '/' || c == '*' || c == '=';
}

bool Lexer::GetNextToken(Token& token)
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
            std::string str;
            cur_pos++;
            while (buf[cur_pos] != '"')
                str += buf[cur_pos++];
            token.type = STRING;
            token.val = str;
            cur_pos++; // Skip '"'
            return true;
        }
        else if (buf[cur_pos] >= '0' && buf[cur_pos] <= '9')
        {
            if (buf[cur_pos+1] == 'x' || buf[cur_pos+1] == 'X')
            {
                printf("HEX\n");
                exit(1);
            }
            else if (buf[cur_pos+1] == 'b' || buf[cur_pos+1] == 'B')
            {
                printf("BINARY\n");
                exit(1);
            }
            else
            {
                // Decimal
                int val = 0;
                bool firstTime = true;
                while (buf[cur_pos] >= '0' && buf[cur_pos] <= '9')
                {
                    if (!firstTime)
                        val *= 10;
                    else
                        firstTime = false;
                    val += (buf[cur_pos++] - '0');
                }
                token.type = Types::INTEGER;
                token.val = std::to_string(val);
                return true;
            }
        }
        else if (IsPunctuation(buf[cur_pos]))
        {
            token.type = PUNCTUATION;
            token.val.clear();
            token.val.push_back(buf[cur_pos++]);
            return true;
        }
        else if (IsOperation(buf[cur_pos]))
        {
            token.type = OPERATION;
            token.val.clear();
            token.val.push_back(buf[cur_pos++]);
            return true;
        }
        else
        {
            std::string tok;
            while (buf[cur_pos] > 32 && !IsPunctuation(buf[cur_pos]))
                tok += buf[cur_pos++];
            token.type = Types::IDENTIFIER;
            token.val = tok;
            return true;
        }

        return true;
    }

    return false;
}

bool Lexer::PeekNextToken(Token &token)
{
    size_t pos = cur_pos;

    while (buf[pos])
    {
        // Skip whitespace and invalid chars
_skip_space:
        while (buf[pos] <= 32 && buf[pos])
        {
            pos++;
            goto _skip_space;
        }
        if (buf[pos] == '/' && buf[pos+1] == '/')
        {
            while (buf[pos] != '\n')
                pos++;
            goto _skip_space;
        }
        if (!buf[pos])
            return false;
        
        if (buf[pos] == '"')
        {
            std::string str;
            pos++;
            while (buf[pos] != '"')
                str += buf[pos++];
            token.type = STRING;
            token.val = str;
            pos++; // Skip '"'
            return true;
        }
        else if (buf[pos] >= '0' && buf[pos] <= '9')
        {
            if (buf[pos+1] == 'x' || buf[pos+1] == 'X')
            {
                printf("HEX\n");
                exit(1);
            }
            else if (buf[pos+1] == 'b' || buf[pos+1] == 'B')
            {
                printf("BINARY\n");
                exit(1);
            }
            else
            {
                // Decimal
                int val;
                bool firstTime = true;
                while (buf[pos] >= '0' && buf[pos] <= '9')
                {
                    if (!firstTime)
                        val *= 10;
                    else
                        firstTime = false;
                    val += (buf[pos++] - '0');
                }
                token.type = Types::INTEGER;
                token.val = std::to_string(val);
                return true;
            }
        }
        else if (IsPunctuation(buf[pos]))
        {
            token.type = PUNCTUATION;
            token.val.clear();
            token.val.push_back(buf[pos++]);
            return true;
        }
        else if (IsOperation(buf[pos]))
        {
            token.type = OPERATION;
            token.val.clear();
            token.val.push_back(buf[pos++]);
            return true;
        }
        else
        {
            std::string tok;
            while (buf[pos] > 32 && !IsPunctuation(buf[pos]))
                tok += buf[pos++];
            token.type = Types::IDENTIFIER;
            token.val = tok;
            return true;
        }

        return true;
    }

    return false;
}
