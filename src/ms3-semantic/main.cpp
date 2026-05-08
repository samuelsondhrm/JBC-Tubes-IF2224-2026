#include "../ms1-lexer/lexer/lexer.hpp"
#include "../ms1-lexer/util/file.hpp"
#include "../ms2-parser/Parser.hpp"
#include "builder/ASTBuilder.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "printer/DecoratedASTPrinter.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./arion-semantic <input.txt> [output.txt]\n";
        return 1;
    }
    std::string src = FileUtil::readFile(argv[1]);

    // Fase MS1 + MS2
    Lexer lexer(src);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    ParseNode* parseTree = parser.parse();

    if (parser.hasError()) {
        for (const auto& e : parser.errors())
            std::cerr << e.message << "\n";
        delete parseTree;
        return 1;
    }

    // Fase MS3
    ASTBuilder builder;
    ASTNode* ast = builder.build(parseTree);   // Fase 1

    SemanticAnalyzer sa;
    sa.analyze(ast);                            // Fase 2 (annotate in-place)

    // Output
    DecoratedASTPrinter printer;
    printer.print(ast, std::cout);
    sa.printSymbolTables(std::cout);

    delete parseTree;
    // Note: in a real implementation we should delete the AST recursively here.
    return sa.hasError() ? 1 : 0;
}
