#include "Parser.hpp"

/**
 * Grammar: const-declaration → constsy + (ident + eql + constant + semicolon)+
 */
ParseNode* Parser::parseConstDeclaration() {
    ParseNode* node = new ParseNode("<const-declaration>");

    node->addChild(makeTerminal(expect(TokenType::KW_CONST, "const-declaration")));
    do {
        node->addChild(makeTerminal(expect(TokenType::IDENT, "const-declaration")));        
        node->addChild(makeTerminal(expect(TokenType::EQL, "const-declaration")));        
        node->addChild(parseConstant());        
        node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "const-declaration"))); 
    } while (check(TokenType::IDENT)); 

    return node;
}


/**
 * Grammar: constant → charcon | string | [(plus | minus)? + (ident | intcon | realcon)]
 */
ParseNode* Parser::parseConstant() {
    ParseNode* node = new ParseNode("<constant>");

    if (check(TokenType::CHARCON) || check(TokenType::STRING)) {
        node->addChild(makeTerminal(consume()));
    } 
    else {
        if (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
            node->addChild(makeTerminal(consume()));
        }
        if (check(TokenType::IDENT) || check(TokenType::INTCON) || check(TokenType::REALCON)) {
            node->addChild(makeTerminal(consume()));
        }
        else {
            error(
                "expected charcon, string, ident, intcon, or realcon in constant", 
                current()
            );
        }
    }

    return node;
}

/**
 * Grammar: type-declaration → typesy + (ident + eql + type + semicolon)+
 */
ParseNode* Parser::parseTypeDeclaration() {
    ParseNode* node = new ParseNode("<type-declaration>");

    node->addChild(makeTerminal(expect(TokenType::KW_TYPE, "type-declaration")));

    do {
        node->addChild(makeTerminal(expect(TokenType::IDENT, "type-declaration")));
        node->addChild(makeTerminal(expect(TokenType::EQL, "type-declaration")));
        node->addChild(parseType());        
        node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "type-declaration")));
        
    } while (check(TokenType::IDENT));

    return node;
}

/**
 * Grammar: type → ident | array-type | range | enumerated | record-type
 */
ParseNode* Parser::parseType() {
    ParseNode* node = new ParseNode("<type>");

    if (check(TokenType::KW_ARRAY)) {
        node->addChild(parseArrayType());
    } 
    else if (check(TokenType::LPAR)) {
        node->addChild(parseEnumerated());
    } 
    else if (check(TokenType::KW_RECORD)) {
        node->addChild(parseRecordType());
    } 
    else {
        // Membedakan 'ident' dengan 'range'
        if (check(TokenType::IDENT)) {
            TokenType next = peekAt(1).type;
            // Jika setelahnya adalah delimiter penutup, ini adalah 'ident'
            if (next == TokenType::SEMICOLON || next == TokenType::RBRACK || 
                next == TokenType::RPAR || next == TokenType::KW_END) {
                node->addChild(makeTerminal(consume()));
                return node;
            }
        }
        // Jika bukan 'ident', token membentuk 'range'
        ParseNode* exprNode = parseExpression();
        node->addChild(parseRange(exprNode));
    }

    return node;
}

/**
 * Grammar: array-type → arraysy + lbrack + (range | ident) + rbrack + ofsy + type
 */
ParseNode* Parser::parseArrayType() {
    ParseNode* node = new ParseNode("<array-type>");

    node->addChild(makeTerminal(expect(TokenType::KW_ARRAY, "array-type")));
    node->addChild(makeTerminal(expect(TokenType::LBRACK, "array-type")));
    // 'ident'
    if (check(TokenType::IDENT) && peekAt(1).type == TokenType::RBRACK) {
        node->addChild(makeTerminal(consume()));
    }
    // 'range'
    else {
        ParseNode* exprNode = parseExpression();
        node->addChild(parseRange(exprNode));
    }
    node->addChild(makeTerminal(expect(TokenType::RBRACK, "array-type")));
    node->addChild(makeTerminal(expect(TokenType::KW_OF, "array-type")));
    node->addChild(parseType());

    return node;
}

/**
 * Grammar: range → expression + period + period + expression
 */
ParseNode* Parser::parseRange(ParseNode* firstExpr) {
    ParseNode* node = new ParseNode("<range>");

    node->addChild(firstExpr);
    node->addChild(makeTerminal(expect(TokenType::PERIOD, "range")));
    node->addChild(makeTerminal(expect(TokenType::PERIOD, "range")));
    node->addChild(parseExpression());

    return node;
}

/**
 * Grammar: enumerated → lpar + ident + (comma + ident)* + rpar
 */
ParseNode* Parser::parseEnumerated() {
    ParseNode* node = new ParseNode("<enumerated>");

    node->addChild(makeTerminal(expect(TokenType::LPAR, "enumerated")));
    node->addChild(makeTerminal(expect(TokenType::IDENT, "enumerated")));
    while (check(TokenType::COMMA)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(makeTerminal(expect(TokenType::IDENT, "enumerated")));
    }
    node->addChild(makeTerminal(expect(TokenType::RPAR, "enumerated")));

    return node;
}

/**
 * Grammar: record-type → recordsy + field-list + endsy
 */
ParseNode* Parser::parseRecordType() {
    ParseNode* node = new ParseNode("<record-type>");

    node->addChild(makeTerminal(expect(TokenType::KW_RECORD, "record-type")));
    node->addChild(parseFieldList());
    node->addChild(makeTerminal(expect(TokenType::KW_END, "record-type")));

    return node;
}

/**
 * Grammar: field-list → field-part + (semicolon + field-part)*
 */
ParseNode* Parser::parseFieldList() {
    ParseNode* node = new ParseNode("<field-list>");

    node->addChild(parseFieldPart());
    while (check(TokenType::SEMICOLON)) {
        // Jika saat ini 'semicolon' dan setelahnya 'end', ini adalah field-list terakhir
        if (peekAt(1).type == TokenType::KW_END) {
            node->addChild(makeTerminal(consume()));
            break;
        }
        node->addChild(makeTerminal(consume()));
        node->addChild(parseFieldPart());
    }

    return node;
}

/**
 * Grammar: field-part → identifier-list + colon + type
 */
ParseNode* Parser::parseFieldPart() {
    ParseNode* node = new ParseNode("<field-part>");

    node->addChild(parseIdentifierList());
    node->addChild(makeTerminal(expect(TokenType::COLON, "field-part")));
    node->addChild(parseType());

    return node;
}

/**
 * Grammar: var-declaration → varsy + (identifier-list + colon + type + semicolon)+
 */
ParseNode* Parser::parseVarDeclaration() {
    ParseNode* node = new ParseNode("<var-declaration>");

    node->addChild(makeTerminal(expect(TokenType::KW_VAR, "var-declaration")));
    do {
        node->addChild(parseIdentifierList());
        node->addChild(makeTerminal(expect(TokenType::COLON, "var-declaration")));
        node->addChild(parseType());
        node->addChild(makeTerminal(expect(TokenType::SEMICOLON, "var-declaration")));
    } while (check(TokenType::IDENT));

    return node;
}

/**
 * Grammar: identifier-list → ident + (comma + ident)*
 */
ParseNode* Parser::parseIdentifierList() {
    ParseNode* node = new ParseNode("<identifier-list>");

    node->addChild(makeTerminal(expect(TokenType::IDENT, "identifier-list")));
    while (check(TokenType::COMMA)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(makeTerminal(expect(TokenType::IDENT, "identifier-list")));
    }

    return node;
}