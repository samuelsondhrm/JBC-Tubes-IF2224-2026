#pragma once
#include <string>
#include <vector>
#include "../token/token.hpp"
#include "../dfa/dfa.hpp"

using namespace std;

/**
 * Lexer
 * 
 * Langkah:
 * - Ambil string
 * - Ekspos nextToken()
 * - Ekspos tokenize()
 */

class Lexer {
public:
    explicit Lexer(const string& src);

    // Return next token, serahkan ke DFA
    Token nextToken();

    // Return all tokens, termasuk EOF.
    vector<Token> tokenize();

    bool isEOF() const;

private:
    DFA dfa;
};
