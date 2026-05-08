#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {}

ParseNode* Parser::parse() {
    return parseProgram();
}

const Token& Parser::current() const {
    if (pos_ >= tokens_.size()) return EOF_TOKEN;
    return tokens_.at(pos_);
}

const Token& Parser::peekAt(size_t offset) const {
    if (pos_ + offset >= tokens_.size()) return EOF_TOKEN;
    return tokens_.at(pos_ + offset);
}

bool Parser::check(TokenType t) const {
    return current().type == t;
}

bool Parser::checkAt(size_t offset, TokenType t) const {
    return peekAt(offset).type == t;
}

Token Parser::consume() {
    if (pos_ >= tokens_.size()) return EOF_TOKEN;
    return tokens_[pos_++];
}

Token Parser::expect(TokenType t, const std::string& context) {
    if (!check(t)) {
        error("in " + context + ": expected " + Token(t, "", -1).tokenTypeName() +
              ", got " + current().tokenTypeName(), current());
        synchronize();
        if (!check(t)) {
            return Token(t, "<missing>", current().line);
        }
    }
    return consume();
}

ParseNode* Parser::makeTerminal(const Token& tok) {
    bool hasValue = tok.type == TokenType::IDENT ||
                    tok.type == TokenType::INTCON ||
                    tok.type == TokenType::REALCON ||
                    tok.type == TokenType::CHARCON ||
                    tok.type == TokenType::STRING ||
                    tok.type == TokenType::UNKNOWN;
    ParseNode* node = new ParseNode(tok.tokenTypeName(), hasValue ? tok.value : "");
    node->isTerminal = true;
    node->line = tok.line;
    return node;
}

void Parser::error(const std::string& msg, const Token& at) {
    errors_.push_back({
        at.line,
        "Syntax error at line " + std::to_string(at.line) +
        ": " + msg +
        " (got '" + at.value + "')"
    });
}

void Parser::synchronize() {
    static const TokenType syncSet[] = {
        TokenType::SEMICOLON, TokenType::KW_END, TokenType::KW_BEGIN,
        TokenType::KW_ELSE,  TokenType::KW_THEN, TokenType::KW_DO,
        TokenType::KW_UNTIL, TokenType::PERIOD,  TokenType::RPAR,
        TokenType::RBRACK,   TokenType::KW_EOF
    };
    while (!check(TokenType::KW_EOF)) {
        for (TokenType t : syncSet) {
            if (check(t)) return;
        }
        consume();
    }
}