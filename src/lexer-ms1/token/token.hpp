#pragma once 
#include <string> 

using namespace std; 
enum class TokenType { 
	// Literals
    INTCON, REALCON, CHARCON, STRING,
 
    // Arithmetic operators
    OP_PLUS, OP_MINUS, OP_TIMES, OP_IDIV, OP_RDIV, OP_MOD,
 
    // Logical operators
    OP_AND, OP_OR, OP_NOT,
 
    // Comparison operators
    EQL, NEQ, GTR, GEQ, LSS, LEQ,
 
    // Delimiters
    LPAR, RPAR, LBRACK, RBRACK,
    COMMA, SEMICOLON, PERIOD, COLON, BECOMES,
 
    // Identifier & comment
    IDENT, COMMENT,
 
    // Keywords: declaration
    KW_CONST, KW_TYPE, KW_VAR, KW_FUNCTION, KW_PROCEDURE,
    KW_ARRAY, KW_RECORD, KW_PROGRAM,
 
    // Keywords: control flow
    KW_BEGIN, KW_END,
    KW_IF, KW_THEN, KW_ELSE,
    KW_WHILE, KW_DO,
    KW_FOR, KW_TO, KW_DOWNTO,
    KW_REPEAT, KW_UNTIL,
    KW_CASE, KW_OF,
 
    // Special
    KW_EOF,
    UNKNOWN   // unrecognised character, for error reporting
};

class Token { 
	public: 
		TokenType type; 
		string value; 
		int line;

		Token(TokenType type, string value, int line);

		string tokenTypeName() const;

		string toString() const;
};


