#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "../ms1-lexer/token/token.hpp"
#include "ParseNode.hpp"

// Thrown on any syntax error. Caught in main() to print the message and exit.
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

// Recursive Descent Parser for the Arion language.
//
// Entry point: Parser p(tokens); ParseNode* tree = p.parse();
//
// Each grammar rule has exactly one parseXxx() method.
// Methods are grouped by team member ownership.
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // Entry point. Returns the root <program> node.
    // Throws ParseError on syntax error.
    ParseNode* parse();

private:
    std::vector<Token> tokens;
    size_t             pos;     // index of current token

    // ── Token utilities ──────────────────────────────────────────────────────

    // Current token (does not advance pos).
    const Token& current() const;

    // Look ahead without advancing. peekAt(0) == current().
    const Token& peekAt(size_t offset) const;

    // Returns true if current token has the given type.
    bool check(TokenType t) const;

    // Returns true if the token at pos+offset has the given type.
    bool checkAt(size_t offset, TokenType t) const;

    // Consume and return the current token, advance pos.
    Token consume();

    // Consume current token if its type matches t.
    // Throws ParseError with a helpful message if it doesn't.
    Token expect(TokenType t, const std::string& context);

    // Wrap a terminal token in a ParseNode.
    // Tokens that carry a value (ident, intcon, realcon, charcon, string,
    // comment, unknown) set node.value = tok.value.
    // All other tokens leave node.value = "".
    ParseNode* makeTerminal(const Token& tok);

    // Throw a ParseError with line information.
    [[noreturn]] void error(const std::string& msg, const Token& at);


    // ── Grammar rules ─────────────────────────────────────────────────────
    // Owner tags match CONTEXT_<NAME>.md in the repository root.

    // [Samuelson] — Top-level & structural
    ParseNode* parseProgram();
    ParseNode* parseProgramHeader();
    ParseNode* parseDeclarationPart();
    ParseNode* parseBlock();
    ParseNode* parseCompoundStatement();
    ParseNode* parseStatementList();
    ParseNode* parseFormalParameterList();
    ParseNode* parseParameterGroup();

    // [Niko] — Declarations
    ParseNode* parseConstDeclaration();
    ParseNode* parseConstant();
    ParseNode* parseTypeDeclaration();
    ParseNode* parseType();
    ParseNode* parseArrayType();
    ParseNode* parseRange(ParseNode* firstExpr);  // firstExpr already parsed
    ParseNode* parseEnumerated();
    ParseNode* parseRecordType();
    ParseNode* parseFieldList();
    ParseNode* parseFieldPart();
    ParseNode* parseVarDeclaration();
    ParseNode* parseIdentifierList();

    // [Reinsen] — Expressions, subprograms, error framework
    ParseNode* parseSubprogramDeclaration();
    ParseNode* parseProcedureDeclaration();
    ParseNode* parseFunctionDeclaration();
    ParseNode* parseExpression();
    ParseNode* parseSimpleExpression();
    ParseNode* parseTerm();
    ParseNode* parseFactor();
    ParseNode* parseParameterList();

    // [Aurelia] — Statements
    // Note: parseStatement reads one token ahead when it sees ident.
    ParseNode* parseStatement();
    ParseNode* parseAssignmentStatement(const Token& identTok);
    ParseNode* parseIfStatement();
    ParseNode* parseCaseStatement();
    ParseNode* parseCaseBlock();
    ParseNode* parseWhileStatement();
    ParseNode* parseRepeatStatement();
    ParseNode* parseForStatement();
    ParseNode* parseProcedureFunctionCall(const Token& identTok);
};
