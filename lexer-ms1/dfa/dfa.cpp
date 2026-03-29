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

            // Q_ALPHA: build identifier or keyword
            case State::Q_ALPHA: {
                if (isalnum(static_cast<unsigned char>(c))) {
                    lexeme += c; // stay in Q_ALPHA
                } else {
                    // Current char belongs to the next token
                    retract();

                    // Check if lexeme is a keyword
                    TokenType t;
                    if (!symTable.lookup(lexeme, t)) {
                        t = TokenType::IDENT;
                    }
                    return Token(t, lexeme, line);
                }
                break;
            }

            // Q_INT: build integer literal
            case State::Q_INT: {
                if (isdigit(static_cast<unsigned char>(c))) {
                    lexeme += c;  // stay in Q_INT
                } else if (c == '.') {
                    // becomes real only if next char is a digit
                    // Example: "5.14" -> REALCON, "5." -> INTCON + PERIOD
                    if (isdigit(static_cast<unsigned char>(peek()))) {
                        lexeme  += c;  // keep '.' in lexeme
                        curState = State::Q_REAL;
                    } else {
                        retract();  // put '.' back as PERIOD
                        return Token(TokenType::INTCON, lexeme, line);
                    }
                } else {
                    retract();
                    return Token(TokenType::INTCON, lexeme, line);
                }
                break;
            }

            // Q_REAL: build real literal after decimal point
            case State::Q_REAL: {
                if (isdigit(static_cast<unsigned char>(c))) {
                    lexeme += c;  // stay in Q_REAL
                } else {
                    retract();
                    return Token(TokenType::REALCON, lexeme, line);
                }
                break;
            }

            // Q_STR: inside single-quoted literal
            // charcon: exactly 1 char between quotes
            // string: 0 or >=2 chars between quotes
            // output keeps the surrounding quotes
            case State::Q_STR: {
                if (c == '\0' || c == '\n') {
                    // Unclosed literal
                    return Token(TokenType::UNKNOWN, "'" + lexeme, line);
                }
                if (c == '\'') {
                    // Closing quote found
                    string literal = "'" + lexeme + "'";
                    if (lexeme.size() == 1) {
                        return Token(TokenType::CHARCON, literal, line);
                    } else {
                        return Token(TokenType::STRING, literal, line);
                    }
                }
                lexeme += c;  // keep literal body only
                break;
            }

            // Q_COLON: seen ':'
            case State::Q_COLON: {
                if (c == '=') {
                    return Token(TokenType::BECOMES, ":=", line);
                }
                retract();
                return Token(TokenType::COLON, ":", line);
            }

            // Q_LT: seen '<'
            case State::Q_LT: {
                if (c == '=') return Token(TokenType::LEQ, "<=", line);
                if (c == '>') return Token(TokenType::NEQ, "<>", line);
                retract();
                return Token(TokenType::LSS, "<", line);
            }

            // Q_GT: seen '>'
            case State::Q_GT: {
                if (c == '=') return Token(TokenType::GEQ, ">=", line);
                retract();
                return Token(TokenType::GTR, ">", line);
            }

            // Q_EQL: seen first '='
            // '=' alone is invalid; only '==' is valid.
            case State::Q_EQL: {
                if (c == '=') return Token(TokenType::EQL, "==", line);
                retract();
                return Token(TokenType::UNKNOWN, "=", line);
            }

            // Q_LPAR: seen '('
            case State::Q_LPAR: {
                if (c == '*') {
                    // Start of (* *) comment
                    lexeme   = "(*";
                    curState = State::Q_CMT_PAREN;
                } else {
                    retract();
                    return Token(TokenType::LPAR, "(", line);
                }
                break;
            }

            // Q_CMT_BRACE: inside { } comment
            case State::Q_CMT_BRACE: {
                if (c == '\0') {
                    // EOF before closing '}'
                    return Token(TokenType::UNKNOWN, lexeme, line);
                }
                lexeme += c;
                if (c == '}') {
                    return Token(TokenType::COMMENT, lexeme, line);
                }
                break;
            }

            // Q_CMT_PAREN: inside (* *) comment
            case State::Q_CMT_PAREN: {
                if (c == '\0') {
                    return Token(TokenType::UNKNOWN, lexeme, line);
                }
                lexeme += c;
                if (c == '*') {
                    curState = State::Q_CMT_PAREN_STAR;
                }
                break;
            }

            // Q_CMT_PAREN_STAR: seen '*' in comment, check close
            case State::Q_CMT_PAREN_STAR: {
                if (c == '\0') {
                    return Token(TokenType::UNKNOWN, lexeme, line);
                }
                lexeme += c;
                if (c == ')') {
                    // "*)" found, comment done
                    return Token(TokenType::COMMENT, lexeme, line);
                }
                if (c == '*') {
                    // Another '*', stay in this state
                } else {
                    // Not closing yet, go back to comment body
                    curState = State::Q_CMT_PAREN;
                }
                break;
            }

        }
    }
}