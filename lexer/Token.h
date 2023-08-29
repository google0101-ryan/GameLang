#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

enum Types
{
    PUNCTUATION, // (, ), {, }, @, ;
    INTEGER,
    STRING,
    IDENTIFIER, // Functions and variables
};

struct Token
{
    Types type;
    std::string val;
};