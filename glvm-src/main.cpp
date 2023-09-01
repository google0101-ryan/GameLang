#include "glvm.h"

#include <fstream>
#include <cstdio>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <infile> [outfile]\n", argv[0]);
        return -1;
    }

    std::ifstream infile(argv[1], std::ios::binary | std::ios::ate);
    size_t size = infile.tellg();
    infile.seekg(0, std::ios::beg);
    uint8_t* buf = new uint8_t[size];
    infile.read((char*)buf, size);
    infile.close();

    GLVM vm(buf);
    vm.DoInterpret();

    delete[] buf;
    return 0;
}