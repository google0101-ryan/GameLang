#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

#define ERROR(x, ...) \
{ \
printf(x, ##__VA_ARGS__); \
exit(1); \
}

enum Types
{
    PUNCTUATION, // (, ), {, }, @, ;
    INTEGER,
    STRING,
    IDENTIFIER, // Functions and variables
    OPERATION,
};

static const char* GetTokenName(Types type)
{
    switch (type)
    {
    case PUNCTUATION:
        return "PUNCTUATION";
    case INTEGER:
        return "INTEGER";
    case STRING:
        return "STRING";
    case IDENTIFIER:
        return "IDENTIFIER";
    case OPERATION:
        return "OPERATION";
    default:
        return "UNKNOWN";
    }
}

struct Token
{
    Types type;
    std::string val;
};