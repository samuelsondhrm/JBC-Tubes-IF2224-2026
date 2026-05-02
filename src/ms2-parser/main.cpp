#include "Parser.hpp"
#include "TreePrinter.hpp"
#include "../ms1-lexer/lexer/lexer.hpp"
#include "../ms1-lexer/util/file.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./arion-parser <input.txt> [output.txt]" << "\n";
        return 1;
    }

    std::string src;
    try {
        src = FileUtil::readFile(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    Lexer lexer(src);
    std::vector<Token> tokens = lexer.tokenize();

    try {
        Parser parser(tokens);
        ParseNode* tree = parser.parse();

        if (argc >= 3) {
            std::ofstream outFile(argv[2]);
            printTree(tree, outFile);
            outFile.close();
            std::cout << "Parse tree saved at " << argv[2] << std::endl;
        }
        printTree(tree, std::cout);
        delete tree;
    } catch (const ParseError& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}