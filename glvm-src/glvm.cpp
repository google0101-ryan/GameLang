#include "glvm.h"
#include <string.h>
#include <cstdio>
#include <cstdlib>

struct [[gnu::packed]] GlcHeader
{
    char magic[6]; // GLANG\0
    uint32_t strPtrTableOffs;
};

struct ReturnFrame
{
    uint32_t addr;
};

std::vector<ReturnFrame> returnFrames;

void GLVM::Println()
{
    uint32_t type = stack[stackPointer+1];
    stackPointer++;

    if (type == 0xEE)
    {
        uint32_t strIndex = stack[stackPointer+1];
        stackPointer++;
        printf("%s\n", (const char*)(data+strPtrTable[strIndex]));
    }
    else
    {
        uint32_t varIndex = stack[stackPointer+1];
        stackPointer++;
        printf("%s\n", vars[varIndex].value.c_str());
    }
}

void GLVM::InitIntVar()
{
    uint32_t index = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;
    uint32_t val = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;

    vars[index].value = std::to_string(val);
}

void GLVM::InitStringVar()
{
    uint32_t index = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;
    uint32_t strPtrIndex = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;

    vars[index].value = (const char*)(data + strPtrTable[strPtrIndex]);
}

void GLVM::SystemCall()
{
    uint8_t code = codeMem[instructionPointer++];

    switch (code)
    {
    case 0x00:
        Println();
        break;
    default:
        printf("Unknown system call %d\n", code);
        exit(1);
    }
}

void GLVM::Return()
{
    if (returnFrames.empty())
    {
        printf("Execution finished\n");
        doneExecuting = true;
    }
    else
    {
        instructionPointer = returnFrames.back().addr;
        returnFrames.pop_back();
    }
}

void GLVM::PushStringLiteral()
{
    // This pushes the index of a string to the stack
    uint32_t index = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;

    stack[stackPointer--] = index; 
    stack[stackPointer--] = 0xEE; // String literal identifier
}

void GLVM::PushVariable()
{
    uint32_t index = *(uint32_t*)&codeMem[instructionPointer];
    instructionPointer += 4;

    stack[stackPointer--] = index;
    stack[stackPointer--] = 0xAA; // variable identifier
}

GLVM::GLVM(uint8_t *data)
: data(data)
{
    GlcHeader* hdr = (GlcHeader*)data;
    vars = new Variable[MAX_VARS];

    if (strncmp(hdr->magic, "GLANG", 6))
    {
        printf("ERROR: GLVM: Expected magic value GLANG, got %s\n", hdr->magic);
        return;
    }

    strPtrTable = (uint32_t*)(data + hdr->strPtrTableOffs);
    codeMem = data+sizeof(GlcHeader);

    Reset();
}

void GLVM::Reset()
{
    stackPointer = STACK_SIZE-1;
    instructionPointer = 0;
    memset(stack, 0, sizeof(stack));
}

bool GLVM::DoInterpret()
{
    bool shouldYield = false;

    while (!shouldYield && !doneExecuting)
    {
        uint8_t opcode = codeMem[instructionPointer++];

        switch (opcode)
        {
        case 0x00:
            InitIntVar();
            break;
        case 0x01:
            InitStringVar();
            break;
        case 0x02:
            SystemCall();
            break;
        case 0x03:
            Return();
            break;
        case 0x04:
            PushStringLiteral();
            break;
        case 0x05:
            PushVariable();
            break;
        default:
            printf("Unknown opcode 0x%02x\n", opcode);
            exit(1);
        }
    }

    return !doneExecuting;
}
