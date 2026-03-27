#pragma once 
#include <string> 

using namespace std; 
enum class TokenType { 
	INTCON, REALCON, CHARCON, STRING,
 	OP_PLUS, OP_MINUS, OP_TIMES, OP_IDIV, OP_RDIV, OP_MOD,
    	OP_AND, OP_OR, OP_NOT,
	EQL, NEQ, GTR, GEQ, LSS, LEQ,
    	LPAR, RPAR, LBRACK, RBRACK,
    	COMMA, SEMICOLON, PERIOD, COLON, BECOMES,
    	IDENT, COMMENT,
    	KW_CONST, KW_TYPE, KW_VAR, KW_FUNCTION, KW_PROCEDURE,
    	KW_ARRAY, KW_RECORD, KW_PROGRAM,
    	KW_BEGIN, KW_END, KW_IF, KW_THEN, KW_ELSE,
    	KW_WHILE, KW_DO, KW_FOR, KW_TO, KW_DOWNTO,
    	KW_REPEAT, KW_UNTIL, KW_CASE, KW_OF,
    	KW_EOF
};

class Token { 
	public: 
		TokenType type; 
		string value; 
		int line; 
		Token(TokenType type, string value, int line);
};


