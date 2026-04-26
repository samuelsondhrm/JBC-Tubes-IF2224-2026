#include "Parser.hpp"


ParseNode* Parser::parseProgram() {
    ParseNode* node = new ParseNode("<program>");

    node->addChild(parseProgramHeader());
    node->addChild(parseDeclarationPart());
    node->addChild(parseCompoundStatement());
    node->addChild(makeTerminal(expect(TokenType::PERIOD, "program")));

    if (!check(TokenType::KW_EOF)) {
        error(
            "unexpected token '" + current().tokenTypeName() +
            "' after end of program",
            current()
        );
    }

    return node;
}

ParseNode* Parser::parseProgramHeader() {
    ParseNode* node = new ParseNode("<program-header>");

    node->addChild(makeTerminal(expect(TokenType::KW_PROGRAM, "program-header")));
    node->addChild(makeTerminal(expect(TokenType::IDENT,      "program-header")));
    node->addChild(makeTerminal(expect(TokenType::SEMICOLON,  "program-header")));

    return node;
}

ParseNode* Parser::parseDeclarationPart() {
    ParseNode* node = new ParseNode("<declaration-part>");

    while (check(TokenType::KW_CONST)) {
        node->addChild(parseConstDeclaration());
    }

    while (check(TokenType::KW_TYPE)) {
        node->addChild(parseTypeDeclaration());
    }

    while (check(TokenType::KW_VAR)) {
        node->addChild(parseVarDeclaration());
    }

    while (check(TokenType::KW_PROCEDURE) || check(TokenType::KW_FUNCTION)) {
        node->addChild(parseSubprogramDeclaration());
    }

    return node;
}

ParseNode* Parser::parseBlock() {
    ParseNode* node = new ParseNode("block");

    node->addChild(parseDeclarationPart());
    node->addChild(parseCompoundStatement());

    return node;
}

ParseNode* Parser::parseCompoundStatement() {
    ParseNode* node = new ParseNode("<compound-statement>");

    node->addChild(makeTerminal(expect(TokenType::KW_BEGIN, "compound-statement")));
    node->addChild(parseStatementList());
    node->addChild(makeTerminal(expect(TokenType::KW_END,   "compound-statement")));

    return node;
}

ParseNode* Parser::parseStatementList() {
    ParseNode* node = new ParseNode("<statement-list>");

    node->addChild(parseStatement());

    while (check(TokenType::SEMICOLON)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseStatement());
    }

    return node;
}


ParseNode* Parser::parseFormalParameterList() {
    ParseNode* node = new ParseNode("<formal-parameter-list>");

    node->addChild(makeTerminal(expect(TokenType::LPAR, "formal-parameter-list")));

    node->addChild(parseParameterGroup());

    while (check(TokenType::SEMICOLON)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseParameterGroup());
    }

    node->addChild(makeTerminal(expect(TokenType::RPAR, "formal-parameter-list")));

    return node;
}

ParseNode* Parser::parseParameterGroup() {
    ParseNode* node = new ParseNode("<parameter-group>");

    node->addChild(parseIdentifierList());
    node->addChild(makeTerminal(expect(TokenType::COLON, "parameter-group")));

    if (check(TokenType::KW_ARRAY)) {
        node->addChild(parseArrayType());
    } else if (check(TokenType::IDENT)) {
        node->addChild(makeTerminal(consume()));
    } else {
        error(
            "expected type name or 'array' in parameter-group, got '" +
            current().tokenTypeName() + "'",
            current()
        );
    }

    return node;
}
