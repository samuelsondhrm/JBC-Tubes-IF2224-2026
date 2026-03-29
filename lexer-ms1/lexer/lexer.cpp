#include "lexer.hpp"

Lexer::Lexer(const string& src) : dfa(src) {}

bool Lexer::isEOF() const {
    return dfa.isEOF();
}

// Return satu token satu waktu
Token Lexer::nextToken() {
    return dfa.nextToken();
}

// Loop sampai EOF
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (true) {
        Token t = dfa.nextToken();
        if (t.type == TokenType::KW_EOF) break;
        tokens.push_back(t);
    }
    return tokens;
}
