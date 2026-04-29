#include "Parser.hpp"

ParseNode* Parser::parseSubprogramDeclaration() {
    ParseNode* node = new ParseNode("<subprogram-declaration>");

    if (check(TokenType::KW_PROCEDURE)) {
        node->addChild(parseProcedureDeclaration());
    } else if (check(TokenType::KW_FUNCTION)) {
        node->addChild(parseFunctionDeclaration());
    } else {
        error(
            "expected 'procedure' or 'function', got '" +
            current().tokenTypeName() + "'",
            current()
        );
    }

    return node;
}

ParseNode* Parser::parseProcedureDeclaration() {
    ParseNode* node = new ParseNode("<procedure-declaration>");

    node->addChild(makeTerminal(expect(TokenType::KW_PROCEDURE, "procedure-declaration")));
    node->addChild(makeTerminal(expect(TokenType::IDENT, "procedure-declaration")));

    if (check(TokenType::LPAR)) {
        node->addChild(parseFormalParameterList());
    }

    node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "procedure-declaration")));
    node->addChild(parseBlock());
    node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "procedure-declaration")));

    return node;
}

ParseNode* Parser::parseFunctionDeclaration() {
    ParseNode* node = new ParseNode("<function-declaration>");

    node->addChild(makeTerminal(expect(TokenType::KW_FUNCTION, "function-declaration")));
    node->addChild(makeTerminal(expect(TokenType::IDENT, "function-declaration")));

    if (check(TokenType::LPAR)) {
        node->addChild(parseFormalParameterList());
    }

    node->addChild(makeTerminal(expect(TokenType::COLON, "function-declaration")));
    node->addChild(makeTerminal(expect(TokenType::IDENT, "function-declaration")));
    node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "function-declaration")));
    node->addChild(parseBlock());
    node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "function-declaration")));

    return node;
}
