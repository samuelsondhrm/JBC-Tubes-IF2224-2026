#include "../ms1-lexer/lexer/lexer.hpp"
#include "../ms1-lexer/util/file.hpp"
#include "../ms2-parser/Parser.hpp"
#include "../ms3-semantic/builder/ASTBuilder.hpp"
#include "../ms3-semantic/semantic/SemanticAnalyzer.hpp"
#include "icg/ICGenerator.hpp"
#include "interpreter/interpreter.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./arion-interpreter <input.txt>\n";
        return 1;
    }
    std::string src = FileUtil::readFile(argv[1]);

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

    ASTBuilder builder;
    ASTNode* ast = builder.build(parseTree);

    SemanticAnalyzer sa;
    sa.analyze(ast);

    if (sa.hasError()) {
        std::cerr << "Semantic Error detected.\n";
        delete parseTree;
        return 1;
    }

    ICGenerator icg(sa.getSymbolTable());
    std::vector<ICInstruction> code = icg.generate(ast);

    // Run interpreter
    Interpreter interpreter(code);
    try {
        interpreter.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        delete parseTree;
        return 1;
    }

    delete parseTree;
    return 0;
}
