#include "Lexer.h"

int main()
{
    Token tk;
    Lexer lex("// This is a comment\nvoid main() {}\n");
    lex.GetNextToken(tk);
    lex.GetNextToken(tk);

    return 0;
}