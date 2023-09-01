#pragma once

#include <string>
#include <vector>
#include <tuple>

enum NodeType
{
    ROOT_NODE,
    FUNCTION,
    VARIABLE,
    FUNCTION_CALL,
    RETURN,
};

struct Node
{
    NodeType type;
    std::vector<Node*> children;
};

struct FuncNode : public Node
{
    std::string name;
    std::vector<std::string> args;
};

struct VarDeclNode : public Node
{
    int index;
    std::string name;
    std::string initialVal;
    enum
    {
        INT,
        STRING,
        FLOAT,
        ENTITY
    } varType;
};

enum ArgType
{
    ARG_STRING,
    ARG_VARIABLE,
};

struct FuncCallArg
{
    std::string val;
    ArgType type;
};

struct FuncCallNode : public Node
{
    std::string name;
    std::vector<FuncCallArg> arg_vals;
};