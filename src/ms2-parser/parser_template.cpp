// parser_template.cpp
//
// This file provides:
//   1. Utility function implementations (everyone reads this).
//   2. The full expression group as an implementation template
//      (parseExpression, parseSimpleExpression, parseTerm, parseFactor).
//
// Every other parseXxx() function follows the same patterns shown here.
// Study these before writing your own functions.

#include "Parser.hpp"
#include <fstream>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {}

ParseNode* Parser::parse() {
    ParseNode* tree = parseProgram();
    return tree;
}

// ─────────────────────────────────────────────────────────────────────────────
// Token utilities
// ─────────────────────────────────────────────────────────────────────────────

// Returns the current token. If pos is past the end, returns the last token
// (which should always be KW_EOF after Lexer::tokenize()).
const Token& Parser::current() const {
    if (pos < tokens.size()) return tokens[pos];
    return tokens.back();
}

// Look ahead without consuming. peekAt(0) == current(), peekAt(1) == next.
const Token& Parser::peekAt(size_t offset) const {
    size_t idx = pos + offset;
    if (idx < tokens.size()) return tokens[idx];
    return tokens.back();
}

bool Parser::check(TokenType t) const {
    return current().type == t;
}

bool Parser::checkAt(size_t offset, TokenType t) const {
    return peekAt(offset).type == t;
}

// Consume and return the current token, then advance.
Token Parser::consume() {
    Token tok = current();
    if (pos < tokens.size()) pos++;
    return tok;
}

// Consume and return the current token only if its type matches t.
// Throws ParseError on mismatch — the most-called utility in the parser.
Token Parser::expect(TokenType t, const std::string& context) {
    if (!check(t)) {
        // Build the expected-token name from a dummy token (no debug cout
        // after the M1 fix removes the cout in Token constructor).
        Token dummy(t, "", 0);
        error(
            "expected '" + dummy.tokenTypeName() +
            "' in " + context +
            ", got '" + current().tokenTypeName() + "'",
            current()
        );
    }
    return consume();
}

// Wrap a token in a leaf ParseNode.
// Only a handful of token types carry meaningful values in the output
// (e.g. ident(Hello), intcon(5)). Everything else shows just the type name.
ParseNode* Parser::makeTerminal(const Token& tok) {
    bool hasValue =
        tok.type == TokenType::IDENT    ||
        tok.type == TokenType::INTCON   ||
        tok.type == TokenType::REALCON  ||
        tok.type == TokenType::CHARCON  ||
        tok.type == TokenType::STRING   ||
        tok.type == TokenType::COMMENT  ||
        tok.type == TokenType::UNKNOWN;

    return new ParseNode(tok.tokenTypeName(), hasValue ? tok.value : "");
}

// Throw a ParseError with line number and message.
void Parser::error(const std::string& msg, const Token& at) {
    throw ParseError(
        "Syntax error at line " + std::to_string(at.line) + ": " + msg
    );
}

// ─────────────────────────────────────────────────────────────────────────────
// TEMPLATE IMPLEMENTATIONS — Expression group
// Owner: Reinsen
//
// PATTERN GUIDE (read before writing any other parseXxx()):
//
//   (A) Mandatory sequence:     node->addChild(parseXxx());
//                               node->addChild(makeTerminal(expect(T, ctx)));
//
//   (B) Optional (zero or one): if (check(T)) { node->addChild(...); }
//
//   (C) One-of-many:            if (check(T1)) { ... }
//                               else if (check(T2)) { ... }
//                               else { error(...); }
//
//   (D) Zero-or-more loop:      while (check(T)) {
//                                   node->addChild(...);
//               }
//
//   (E) One-or-more loop:       // First iteration mandatory, rest via while
//                               do { ... } while (check(T));
// ─────────────────────────────────────────────────────────────────────────────

// Grammar: expression → simple-expression (relational-operator simple-expression)?
//
// Pattern B (optional suffix): the relational comparison is optional.
ParseNode* Parser::parseExpression() {
    ParseNode* node = new ParseNode("<expression>");

    // Mandatory: always one simple-expression
    node->addChild(parseSimpleExpression());

    // Optional: relational-operator + second simple-expression  [Pattern B]
    if (check(TokenType::EQL) || check(TokenType::NEQ) ||
        check(TokenType::GTR) || check(TokenType::GEQ) ||
        check(TokenType::LSS) || check(TokenType::LEQ)) {

        ParseNode* opNode = new ParseNode("<relational-operator>");
        opNode->addChild(makeTerminal(consume())); // consume the operator
        node->addChild(opNode);

        node->addChild(parseSimpleExpression());
    }
    return node;
}

// Grammar: simple-expression → (plus | minus)? term (additive-operator term)*
//
// Pattern B (optional prefix) + Pattern D (zero-or-more suffix loop).
ParseNode* Parser::parseSimpleExpression() {
    ParseNode* node = new ParseNode("<simple-expression>");

    // Optional leading sign  [Pattern B]
    if (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        node->addChild(makeTerminal(consume()));
    }

    // Mandatory: first term
    node->addChild(parseTerm());

    // Zero-or-more: additive-operator term  [Pattern D]
    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS) ||
           check(TokenType::OP_OR)) {

        ParseNode* opNode = new ParseNode("<additive-operator>");
        opNode->addChild(makeTerminal(consume()));
        node->addChild(opNode);

        node->addChild(parseTerm());
    }
    return node;
}

// Grammar: term → factor (multiplicative-operator factor)*
//
// Pattern D (zero-or-more loop after first mandatory factor).
ParseNode* Parser::parseTerm() {
    ParseNode* node = new ParseNode("<term>");

    node->addChild(parseFactor()); // mandatory first factor

    // Zero-or-more: multiplicative-operator factor  [Pattern D]
    while (check(TokenType::OP_TIMES) || check(TokenType::OP_RDIV) ||
           check(TokenType::OP_IDIV)  || check(TokenType::OP_MOD)  ||
           check(TokenType::OP_AND)) {

        ParseNode* opNode = new ParseNode("<multiplicative-operator>");
        opNode->addChild(makeTerminal(consume()));
        node->addChild(opNode);

        node->addChild(parseFactor());
    }
    return node;
}

// Grammar: factor → ident
//                  | intcon | realcon | charcon | string
//                  | lparent expression rparent
//                  | notsy factor
//                  | function-call        (ident lparent parameter-list rparent)
//
// Pattern C (one-of-many alternatives).
// Key lookahead: ident followed by lparent → function-call, otherwise plain ident.
ParseNode* Parser::parseFactor() {
    ParseNode* node = new ParseNode("<factor>");

    if (check(TokenType::IDENT)) {
        Token identTok = consume();

        // Lookahead: is this a function-call?  [one token ahead]
        if (check(TokenType::LPAR)) {
            // function-call: ident ( [parameter-list] )
            ParseNode* callNode = new ParseNode("<function-call>");
            callNode->addChild(makeTerminal(identTok));
            callNode->addChild(makeTerminal(consume())); // lparent

            if (!check(TokenType::RPAR)) {               // optional parameter list
                callNode->addChild(parseParameterList());
            }

            callNode->addChild(makeTerminal(expect(TokenType::RPAR, "function-call")));
            node->addChild(callNode);
        } else {
            // Plain identifier
            node->addChild(makeTerminal(identTok));
        }

    } else if (check(TokenType::INTCON)) {
        node->addChild(makeTerminal(consume()));

    } else if (check(TokenType::REALCON)) {
        node->addChild(makeTerminal(consume()));

    } else if (check(TokenType::CHARCON)) {
        node->addChild(makeTerminal(consume()));

    } else if (check(TokenType::STRING)) {
        node->addChild(makeTerminal(consume()));

    } else if (check(TokenType::LPAR)) {
        // Parenthesised sub-expression: ( expression )
        node->addChild(makeTerminal(consume())); // lparent
        node->addChild(parseExpression());
        node->addChild(makeTerminal(expect(TokenType::RPAR, "factor")));

    } else if (check(TokenType::OP_NOT)) {
        // notsy factor  (recursive, Pattern A then recursion)
        node->addChild(makeTerminal(consume())); // notsy
        node->addChild(parseFactor());

    } else {
        error(
            "unexpected token '" + current().tokenTypeName() +
            "' in factor — expected expression operand",
            current()
        );
    }
    return node;
}

// Grammar: parameter-list → expression (comma expression)*
//
// Pattern E (one-or-more): at least one expression, then comma-separated repeats.
// Called by parseFactor (function-call) and parseProcedureFunctionCall.
ParseNode* Parser::parseParameterList() {
    ParseNode* node = new ParseNode("<parameter-list>");

    node->addChild(parseExpression()); // first expression mandatory

    while (check(TokenType::COMMA)) { // [Pattern D]
        node->addChild(makeTerminal(consume())); // comma
        node->addChild(parseExpression());
    }
    return node;
}
