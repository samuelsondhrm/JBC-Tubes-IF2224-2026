#include "dfa.hpp"
#include <cctype>

using namespace std;

// Constructor
DFA::DFA(const string& src) : source(src), pos(0), line(1) {}

bool DFA::isEOF() const {
    return pos >= static_cast<int>(source.size());
}

// Consume and return the current character, advance char position.
// Returns '\0' at end-of-source. Tracks line number on newlines.
char DFA::advance() {
    if (isEOF()) return '\0';
    char c = source[pos++];
    if (c == '\n') line++;
    return c;
}

// Return the current character without consuming it.
char DFA::peek() const {
    if (isEOF()) return '\0';
    return source[pos];
}

// Step back one character (undo the last advance).
void DFA::retract() {
    if (pos > 0) {
        pos--;
        if (source[pos] == '\n') line--;
    }
}

// Skip all whitespace characters before the next token.
void DFA::skipWhitespace() {
    while (!isEOF() && isspace(static_cast<unsigned char>(peek()))) {
        advance();
    }
}

// nextToken workflow:
//  1. Skip leading whitespace.
//  2. Reset lexeme and currentState to Q0.
//  3. Loop: advance one char, dispatch on currentState via switch.
//      - Non-accepting states transition and continue the loop.
//      - Accepting states return a Token immediately.
Token DFA::nextToken() {
    skipWhitespace();

    if (isEOF()) {
        return Token(TokenType::KW_EOF, "", line);
    }

    string lexeme    = "";
    State  curState  = State::Q0;

    while (true) {
        char c = advance();

        switch (curState) {

            case State::Q0: {

                // Protect once more from EOF
                if (c == '\0') {
                    return Token(TokenType::KW_EOF, "", line);
                }

                // identifier/keyword
                if (isalpha(static_cast<unsigned char>(c))) {
                    lexeme  += c;
                    curState = State::Q_ALPHA;
                    break;
                }

                // integer/real literal
                if (isdigit(static_cast<unsigned char>(c))) {
                    lexeme  += c;
                    curState = State::Q_INT;
                    break;
                }

                // single-quoted string/char literal
                if (c == '\'') {
                    curState = State::Q_STR;
                    break;
                }

                // multi-char operators that need a lookahead
                if (c == ':') { curState = State::Q_COLON; break; }
                if (c == '<') { curState = State::Q_LT;    break; }
                if (c == '>') { curState = State::Q_GT;    break; }
                if (c == '=') { curState = State::Q_EQL;   break; }

                // left-parenthesis that might start a (* *) comment
                if (c == '(') { curState = State::Q_LPAR;  break; }

                // brace comment
                if (c == '{') {
                    lexeme  += c;
                    curState = State::Q_CMT_BRACE;
                    break;
                }

                // single-character tokens
                if (c == '+') return Token(TokenType::OP_PLUS,   "+", line);
                if (c == '-') return Token(TokenType::OP_MINUS,  "-", line);
                if (c == '*') return Token(TokenType::OP_TIMES,  "*", line);
                if (c == '/') return Token(TokenType::OP_RDIV,   "/", line);
                if (c == ')') return Token(TokenType::RPAR,      ")", line);
                if (c == '[') return Token(TokenType::LBRACK,    "[", line);
                if (c == ']') return Token(TokenType::RBRACK,    "]", line);
                if (c == ',') return Token(TokenType::COMMA,     ",", line);
                if (c == ';') return Token(TokenType::SEMICOLON, ";", line);
                if (c == '.') return Token(TokenType::PERIOD,    ".", line);

                // unrecognised character
                return Token(TokenType::UNKNOWN, string(1, c), line);
            }

        }
    }
}