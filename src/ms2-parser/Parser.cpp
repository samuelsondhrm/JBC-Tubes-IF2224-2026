#include "Parser.hpp"

static const Token EOF_TOKEN { TokenType::KW_EOF, "", -1 };

// forward declare ParseNode 
class ParseNode; 

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {} 


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

Token Parser::consume(){ 
    if (pos_ >= tokens_.size()) return EOF_TOKEN; 
    return tokens_[pos_++];
} 

Token Parser::expect(TokenType t, const std::string& context) {
    if (!check(t)){ 
        error("in " + context + ": expected " + Token(t, "", -1).tokenTypeName()+
              ", got " + current().tokenTypeName(), current()); 
    } 
    return consume(); 
} 
//
ParseNode* makeTerminal(const Token& tok)  
{ 
    return ::makeTerminal(tok); 
}

void Parser::error(const std::string& msg, const Token& at) {
    throw ParseError(at,
        "Syntax error at line " + std::to_string(at.line) +
        ": " + msg +
        " (got '" + at.value
        + "')");
}
