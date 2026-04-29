#include "Parser.hpp"

namespace {

bool isRelationalOperator(TokenType type) {
    return type == TokenType::EQL ||
           type == TokenType::NEQ ||
           type == TokenType::GTR ||
           type == TokenType::GEQ ||
           type == TokenType::LSS ||
           type == TokenType::LEQ;
}

bool isAdditiveOperator(TokenType type) {
    return type == TokenType::OP_PLUS ||
           type == TokenType::OP_MINUS ||
           type == TokenType::OP_OR;
}

bool isMultiplicativeOperator(TokenType type) {
    return type == TokenType::OP_TIMES ||
           type == TokenType::OP_RDIV ||
           type == TokenType::OP_IDIV ||
           type == TokenType::OP_MOD ||
           type == TokenType::OP_AND;
}

} // namespace

ParseNode* Parser::parseExpression() {
    ParseNode* node = new ParseNode("<expression>");

    node->addChild(parseSimpleExpression());

    if (isRelationalOperator(current().type)) {
        ParseNode* opNode = new ParseNode("<relational-operator>");
        opNode->addChild(makeTerminal(consume()));
        node->addChild(opNode);

        node->addChild(parseSimpleExpression());
    }

    return node;
}

ParseNode* Parser::parseSimpleExpression() {
    ParseNode* node = new ParseNode("<simple-expression>");

    if (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        node->addChild(makeTerminal(consume()));
    }

    node->addChild(parseTerm());

    while (isAdditiveOperator(current().type)) {
        ParseNode* opNode = new ParseNode("<additive-operator>");
        opNode->addChild(makeTerminal(consume()));
        node->addChild(opNode);

        node->addChild(parseTerm());
    }

    return node;
}

ParseNode* Parser::parseTerm() {
    ParseNode* node = new ParseNode("<term>");

    node->addChild(parseFactor());

    while (isMultiplicativeOperator(current().type)) {
        ParseNode* opNode = new ParseNode("<multiplicative-operator>");
        opNode->addChild(makeTerminal(consume()));
        node->addChild(opNode);

        node->addChild(parseFactor());
    }

    return node;
}

ParseNode* Parser::parseFactor() {
    ParseNode* node = new ParseNode("<factor>");

    if (check(TokenType::IDENT)) {
        Token identTok = consume();

        if (check(TokenType::LPAR)) {
            ParseNode* callNode = new ParseNode("<procedure/function-call>");
            callNode->addChild(makeTerminal(identTok));
            callNode->addChild(makeTerminal(consume()));

            if (!check(TokenType::RPAR)) {
                callNode->addChild(parseParameterList());
            }

            callNode->addChild(makeTerminal(expect(TokenType::RPAR, "procedure/function-call")));
            node->addChild(callNode);
        } else {
            node->addChild(makeTerminal(identTok));
        }

        return node;
    }

    if (check(TokenType::INTCON) ||
        check(TokenType::REALCON) ||
        check(TokenType::CHARCON) ||
        check(TokenType::STRING)) {
        node->addChild(makeTerminal(consume()));
        return node;
    }

    if (check(TokenType::LPAR)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseExpression());
        node->addChild(makeTerminal(expect(TokenType::RPAR, "factor")));
        return node;
    }

    if (check(TokenType::OP_NOT)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseFactor());
        return node;
    }

    error(
        "unexpected token '" + current().tokenTypeName() +
        "' in factor - expected expression operand",
        current()
    );
}

ParseNode* Parser::parseParameterList() {
    ParseNode* node = new ParseNode("<parameter-list>");

    node->addChild(parseExpression());

    while (check(TokenType::COMMA)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseExpression());
    }

    return node;
}
