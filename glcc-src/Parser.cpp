#include "Parser.h"
#include <cassert>

Parser::Parser(std::string buf)
: lex(buf)
{
}

extern bool IsPunctuation(char c);

bool IsFunction(std::string typeName)
{
    return (typeName == "function");
}

bool IsVar(std::string typeName)
{
    return (typeName == "var");
}

void PrintFunction(FuncNode* node)
{
    assert(node->type == FUNCTION);

    printf("Function %s(", node->name.c_str());
    for (int i = 0; i < node->args.size(); i++)
    {
        printf("%s", node->args[i].c_str());
        if (i < (node->args.size()-1))
            printf(", ");
    }
    printf(")\n");
}

void PrintVariable(VarDeclNode* node)
{
    printf("Var \"%s\", initial value = \"%s\"\n", node->name.c_str(), node->initialVal.c_str());
}

void PrintFuncCall(FuncCallNode* node)
{
    printf("Function call \"%s\" with args: ", node->name.c_str());
    for (int i = 0; i < node->arg_vals.size(); i++)
    {
        printf("\"%s\"", node->arg_vals[i].val.c_str());
        if (i < (node->arg_vals.size()-1))
            printf(", ");
    }
    printf("\n");
}

void PrintReturn()
{
    printf("Function return\n");
}

void RecursivelyPrintTree(Node* node, int level)
{
    for (int i = 0; i < level; i++)
        printf("\t");

    switch (node->type)
    {
    case ROOT_NODE:
        printf("(ROOT)\n");
        break;
    case FUNCTION:
        PrintFunction((FuncNode*)node);
        break;
    case VARIABLE:
        PrintVariable((VarDeclNode*)node);
        break;
    case FUNCTION_CALL:
        PrintFuncCall((FuncCallNode*)node);
        break;
    case RETURN:
        PrintReturn();
        break;
    default:
        printf("Unknown AST tree node type %d\n", node->type);
        break;
    }

    for (auto& node : node->children)
    {
        RecursivelyPrintTree(node, level+1);
    }
}

Node* Parser::Parse(std::string outPutFile)
{
    Token token;
    Node* rootNode = new Node;
    Node* current = nullptr;

    std::vector<VarDeclNode*> vars;
    int varIndex = 0;
    VarDeclNode* prevVarNode = nullptr;
    
    rootNode->type = ROOT_NODE;
    FuncNode* mainNode = nullptr;

    while (lex.GetNextToken(token))
    {
        if (IsFunction(token.val))
        {
            lex.GetNextToken(token);
            if (token.type != IDENTIFIER)
                ERROR("ERROR: Expected identifier, got \"%s\"\n", token.val.c_str());
            
            FuncNode* func = new FuncNode;
            func->name = token.val;
            func->type = FUNCTION;

            lex.GetNextToken(token);
            if (token.type != PUNCTUATION || token.val != "(")
                ERROR("Expected \"(\", got \"%s\" instead\n", token.val.c_str());

            while (lex.GetNextToken(token) && token.val != ")")
            {
                // Arguments
                func->args.push_back(token.val);

                if (lex.GetNextToken(token) && token.val != ",")
                    break;
            }

            lex.GetNextToken(token);
            if (token.type != PUNCTUATION || token.val != "{")
                ERROR("Expected \"{\", got \"%s\"\n", token.val.c_str());
            
            rootNode->children.push_back(func);
            current = rootNode->children.back();

            if (func->name == "main")
                mainNode = func;
        }
        else if (token.type == PUNCTUATION && token.val == "}")
        {
            printf("End of function scope!\n");
            current = nullptr;
        }
        else if (IsVar(token.val))
        {
            lex.GetNextToken(token);
            VarDeclNode* var = new VarDeclNode;

            var->type = VARIABLE;
            var->name = token.val;
            var->index = varIndex++;

            vars.push_back(var);

            if (current)
                current->children.push_back(var);
            else
                rootNode->children.push_back(var);
            
            Token peekToken;
            if (lex.PeekNextToken(peekToken) && peekToken.type == OPERATION && peekToken.val == "=")
            {
                lex.GetNextToken(token);
                lex.GetNextToken(token); // The value
                if (token.type != IDENTIFIER && token.type != INTEGER && token.type != STRING)
                    ERROR("Error: expected identifier or constant after \"=\", got \"%s\" instead\n", token.val.c_str());
                
                if (token.type == STRING)
                    var->varType = VarDeclNode::STRING;
                else if (token.type == INTEGER)
                    var->varType = VarDeclNode::INT;
                
                var->initialVal = token.val;
            }
            else
                var->initialVal = "";
            
            printf("New variable \"%s\", index %d, initial value \"%s\"\n", var->name.c_str(), var->index, var->initialVal.c_str());

            if (!lex.GetNextToken(token) || token.val != ";")
                printf("Expected semicolon at end of variable declaration\n");
        }
        else if (token.type == IDENTIFIER && token.val == "return")
        {
            Node* node = new Node;
            node->type = RETURN;

            if (!current)
                ERROR("Unexpected \"return\" outside of function\n");
            
            current->children.push_back(node);

            Token peekToken;
            if (lex.PeekNextToken(peekToken) && (token.type == INTEGER || token.type == STRING))
                ERROR("TODO: Non-void return statements!\n");

            if (!lex.GetNextToken(token) || token.type != PUNCTUATION || token.val != ";")
                    ERROR("Expected ';' at the end of return statement, got \"%s\"\n", token.val.c_str());
        }
        else
        {
            Token peekToken;
            if (lex.PeekNextToken(peekToken) && peekToken.type == PUNCTUATION && peekToken.val == "(")
            {
                FuncCallNode* node = new FuncCallNode;
                node->name = token.val;
                node->type = FUNCTION_CALL;

                printf("Function call: calling %s(", token.val.c_str());

                lex.GetNextToken(token);

                while (lex.GetNextToken(token) && token.val != ")")
                {
                    // Arguments
                    ArgType type;
                    if (token.type == STRING)
                        type = ARG_STRING;
                    else if (token.type == IDENTIFIER)
                        type = ARG_VARIABLE;
                    else
                        ERROR("Unknown arg type %d\n", token.type);

                    node->arg_vals.push_back({.val = token.val, .type = type});

                    if (token.type == STRING)
                        printf("\"");

                    printf("%s", token.val.c_str());

                    if (token.type == STRING)
                        printf("\"");

                    if (lex.GetNextToken(token) && token.val != ",")
                        break;
                    
                    printf(", ");
                }

                printf(")\n");

                if (!lex.GetNextToken(token) || token.type != PUNCTUATION || token.val != ";")
                    ERROR("Expected ';' at the end of function call, got \"%s\"\n", token.val.c_str());
                
                current->children.push_back(node);
            }
            else
            {
                ERROR("Unknown token \"%s\"\n", token.val.c_str());
            }
        }
    }

    RecursivelyPrintTree(rootNode, 0);

    if (!mainNode)
        ERROR("ERROR: No main entry point defined!\n");
    
    // TODO: Dump AST to intermediate file if the user wants it

    return rootNode;
}
