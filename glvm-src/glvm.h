#pragma once

#include <queue>
#include <cstdint>
#include <memory>
#include <string>

#define STACK_SIZE (64*1024) // 64 KB stack per GLVM context
#define MAX_VARS (128*1024)

struct Variable
{
    std::string value;
};

class GLVM
{
private:
    uint8_t* data; // The whole glc file
    uint32_t* strPtrTable; // An array of 32-bit string table pointers

    uint8_t* codeMem; // pointer to the beginning of the code section of the .glc file
    
    uint32_t stack[STACK_SIZE];
    Variable* vars;

    uint32_t instructionPointer;
    uint32_t stackPointer;

    bool doneExecuting = false;

    void Println();

    void InitIntVar(); // 0x00
    void InitStringVar(); // 0x01
    void SystemCall(); // 0x02
    void Return(); // 0x03
    void PushStringLiteral(); // 0x04
    void PushVariable(); // 0x05
public:
    GLVM(uint8_t* data);
    void Reset();

    bool DoInterpret();
};