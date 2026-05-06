#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "../ms1-lexer/token/token.hpp"
#include "ParseNode.hpp"

class ParseError : public std::runtime_error {
private: 
    Token token_; 
    int line_; 

public:
    explicit ParseError(const Token& tok, const std::string& msg) : std::runtime_error(msg), token_(tok), line_(tok.line) {} 
};
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    ParseNode* parse();

private:
    std::vector<Token> tokens_;
    size_t             pos_;     // index of current token
    inline static const Token EOF_TOKEN{TokenType::KW_EOF, "", -1}; 


    const Token& current() const;

    const Token& peekAt(size_t offset) const;

    bool check(TokenType t) const;

    bool checkAt(size_t offset, TokenType t) const;

    Token consume();

    Token expect(TokenType t, const std::string& context);

    ParseNode* makeTerminal(const Token& tok);

	void error(const std::string& msg, const Token& at);


    ParseNode* parseProgram();
    ParseNode* parseProgramHeader();
    ParseNode* parseDeclarationPart();
    ParseNode* parseBlock();
    ParseNode* parseCompoundStatement();
    ParseNode* parseStatementList();
    ParseNode* parseFormalParameterList();
    ParseNode* parseParameterGroup();

    ParseNode* parseConstDeclaration();
    ParseNode* parseConstant();
    ParseNode* parseTypeDeclaration();
    ParseNode* parseType();
    ParseNode* parseArrayType();
    ParseNode* parseRange(ParseNode* firstConst);  // firstConst already parsed
    ParseNode* parseEnumerated();
    ParseNode* parseRecordType();
    ParseNode* parseFieldList();
    ParseNode* parseFieldPart();
    ParseNode* parseVarDeclaration();
    ParseNode* parseIdentifierList();

    ParseNode* parseSubprogramDeclaration();
    ParseNode* parseProcedureDeclaration();
    ParseNode* parseFunctionDeclaration();
    ParseNode* parseExpression();
    ParseNode* parseSimpleExpression();
    ParseNode* parseTerm();
    ParseNode* parseFactor();
    ParseNode* parseParameterList();

    ParseNode* parseStatement();
    ParseNode* parseVariable(const Token& identTok);
    ParseNode* parseComponentVariable();
    ParseNode* parseIndexList();
    ParseNode* parseAssignmentStatement(const Token& identTok);
    ParseNode* parseIfStatement();
    ParseNode* parseCaseStatement();
    ParseNode* parseCaseBlock();
    ParseNode* parseWhileStatement();
    ParseNode* parseRepeatStatement();
    ParseNode* parseForStatement();
    ParseNode* parseProcedureFunctionCall(const Token& identTok);
};
