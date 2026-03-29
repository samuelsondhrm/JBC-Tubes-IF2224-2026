#pragma once
#include <string>
#include "../token/token.hpp"
#include "../lexer/symbolTable.hpp"

using namespace std;

// Q0               → start / reset state
// Q_ALPHA          → building an identifier or keyword
// Q_INT            → building an integer literal
// Q_REAL           → building a real literal (past the '.')
// Q_STR            → inside a single-quoted string / char literal
// Q_COLON          → seen ':', deciding between ':' and ':='
// Q_LT             → seen '<', deciding between '<', '<=', '<>'
// Q_GT             → seen '>', deciding between '>' and '>='
// Q_EQL            → seen first '=', expecting second '=' for '=='
// Q_LPAR           → seen '(', deciding between '(' and '(*' comment
// Q_CMT_BRACE      → inside a { } comment
// Q_CMT_PAREN      → inside a (* *) comment
// Q_CMT_PAREN_STAR → inside (* *) comment, just saw '*' (possible close)
enum class State {
    Q0,
    Q_ALPHA,
    Q_INT,
    Q_REAL,
    Q_STR,
    Q_COLON,
    Q_LT,
    Q_GT,
    Q_EQL,
    Q_LPAR,
    Q_CMT_BRACE,
    Q_CMT_PAREN,
    Q_CMT_PAREN_STAR
};

// DFA pure state machine to consume a token based on the source input until EOF.
class DFA {
public:
    explicit DFA(const string& src);

    Token nextToken();

    bool isEOF() const;

private:
    string      source;
    int         pos;    // current read position
    int         line;   // current line number (1-based, for error reporting)
    SymbolTable symTable;

    char advance();         // read source[pos], increment pos, track newlines
    char peek()   const;    // look at source[pos] without consuming
    void retract();         // step pos back by one (undo last advance)
    void skipWhitespace();  // consume all leading whitespace before a token
};