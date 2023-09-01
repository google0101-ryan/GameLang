#include "Parser.h"
#include "Compiler.h"

#include <sstream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <infile> [outfile]\n", argv[0]);
        return -1;
    }

    std::string outfile_name;
    if (argc > 2)
        outfile_name = argv[2];
    else
    {
        std::filesystem::path path(argv[1]);
        path.replace_extension(".glc");
        outfile_name = path.string();
    }

    std::ifstream infile(argv[1]);
    std::stringstream sstream;
    sstream << infile.rdbuf();

    Parser parser(sstream.str());
    Node* root = parser.Parse("output.gl_ast");

    Compiler compiler(root);
    compiler.Compile(outfile_name);

    return 0;
}