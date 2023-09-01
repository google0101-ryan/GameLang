#include "Parser.h"
#include "Compiler.h"

#include <sstream>
#include <fstream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <infile> [outfile]\n", argv[0]);
        return -1;
    }

    std::ifstream infile(argv[1]);
    std::stringstream sstream;
    sstream << infile.rdbuf();

    Parser parser(sstream.str());
    Node* root = parser.Parse("output.gl_ast");

    Compiler compiler(root);
    compiler.Compile("output.glc");

    return 0;
}