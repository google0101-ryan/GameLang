#include "Compiler.h"
#include "Token.h"
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include <string.h>

Compiler::Compiler(Node *ast_tree, bool should_disassemble)
{
    tree = ast_tree;
    do_disassembly = should_disassemble;
}

enum Ops : uint8_t
{
    InitIntVar = 0x00,
    InitStringVar = 0x01,
    SYSCALL = 0x02,
    RETURN_STATEMENT = 0x03,
    Load_StringLiteral = 0x04, // Push a string literal index onto the stack
    Load_Variable = 0x05,
};

enum Syscalls
{
    PRINTLN,
};

bool IsSystemFunc(std::string funcName)
{
    return (funcName == "println");
}

std::unordered_map<std::string, VarDeclNode*> vars;

std::vector<const char*> stringTable;

void CompileVariable(std::vector<uint8_t>& output, VarDeclNode* node)
{
    printf("Compiling variable %s, type %d\n", node->name.c_str(), node->varType);

    if (node->varType == VarDeclNode::INT)
    {
        uint32_t val = atoi(node->initialVal.c_str());
        output.push_back(InitIntVar);
        for (int i = 0; i < 4; i++)
            output.push_back((node->index >> (i*8)) & 0xFF);
        for (int i = 0; i < 4; i++)
            output.push_back((val >> (i*8)) & 0xFF);
    }
    else if (node->varType == VarDeclNode::STRING)
    {
        output.push_back(InitStringVar);
        for (int i = 0; i < 4; i++)
            output.push_back((node->index >> (i*8)) & 0xFF);
        stringTable.push_back(node->initialVal.c_str());
        uint32_t index = stringTable.size()-1;
        for (int i = 0; i < 4; i++)
            output.push_back((index >> (i*8)) & 0xFF);
    }
    else
        ERROR("Unknown variable type %d\n", node->varType);
    
    vars[node->name] = node;
}

void CompileFuncCall(std::vector<uint8_t>& output, FuncCallNode* node)
{
    if (!IsSystemFunc(node->name))
        ERROR("TODO: Non-system function calls\n");
    
    if (node->name == "println")
    {
        // If the first arg is a string literal
        if (node->arg_vals[0].type == ARG_STRING)
        {
            // Append the string to the string table
            stringTable.push_back(node->arg_vals[0].val.c_str());
            output.push_back(Load_StringLiteral);
            uint32_t val = stringTable.size()-1;
            for (int i = 0; i < 4; i++)
                output.push_back((val >> (i*8)) & 0xFF);
        }
        else
        {
            output.push_back(Load_Variable);
            uint32_t val = vars[node->arg_vals[0].val]->index;
            for (int i = 0; i < 4; i++)
                output.push_back((val >> (i*8)) & 0xFF);
        }

        output.push_back(SYSCALL);
        output.push_back(PRINTLN);
    }
    else
        ERROR("Unknown system function \"%s\"\n", node->name.c_str());
}

void Compiler::Compile(std::string output)
{
    std::vector<uint8_t> outputBuffer;

    Node* mainNode = nullptr;
    for (auto& child : tree->children)
    {
        if (child->type == FUNCTION && ((FuncNode*)child)->name == "main")
        {
            mainNode = child;
        }
        else if (child->type == VARIABLE)
        {
            CompileVariable(outputBuffer, (VarDeclNode*)child);
        }
    }

    if (!mainNode)
        ERROR("No main node found in AST!\n");

    // Compile children of main node (this builds the main function)
    for (auto& child : mainNode->children)
    {
        switch (child->type)
        {
        case VARIABLE:
            CompileVariable(outputBuffer, (VarDeclNode*)child);
            break;
        case FUNCTION_CALL:
            CompileFuncCall(outputBuffer, (FuncCallNode*)child);
            break;
        case RETURN:
            outputBuffer.push_back(RETURN_STATEMENT);
            break;
        default:
            ERROR("Unknown child type %d\n", child->type);
        }
    }

    std::ofstream outfile(output);

    struct [[gnu::packed]] Header
    {
        char magic[6];
        uint32_t strPtrTablePos;
    } header;
    
    uint32_t strPos = sizeof(Header) + outputBuffer.size() + (stringTable.size() * 4);
    
    header.magic[0] = 'G';
    header.magic[1] = 'L';
    header.magic[2] = 'A';
    header.magic[3] = 'N';
    header.magic[4] = 'G';
    header.magic[5] = '\0';
    header.strPtrTablePos = sizeof(Header) + outputBuffer.size();

    outfile.write((const char*)&header, sizeof(Header));

    outfile.write((const char*)outputBuffer.data(), outputBuffer.size());

    for (size_t i = 0; i < stringTable.size(); i++)
    {
        uint32_t val = strPos;
        outfile.write((const char*)&val, 4);
        strPos += strlen(stringTable[i])+1;
    }

    for (auto& str : stringTable)
    {
        outfile.write(str, strlen(str)+1);
    }

    outfile.close();
}