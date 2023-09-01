#pragma once

#include "AST.h"

/***
 * @brief Takes in an AST and compiles it into bytecode
*/
class Compiler
{
private:
    Node* tree;
    bool do_disassembly;
public:
    Compiler(Node* ast_tree, bool should_disassemble = false);

    void Compile(std::string output);
};