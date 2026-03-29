#include "token.hpp" 
#include <iostream>  
using namespace std;


Token::Token(TokenType type, string value, int line) : type(type), value(value), line(line) { 
	cout << "Token constructed" << endl; 
} 

// Maps each TokenType to the string name required by the spec output format.
string Token::tokenTypeName() const {
    switch (type) {
        // Literals
        case TokenType::INTCON:        return "intcon";
        case TokenType::REALCON:       return "realcon";
        case TokenType::CHARCON:       return "charcon";
        case TokenType::STRING:        return "string";
 
        // Arithmetic
        case TokenType::OP_PLUS:       return "plus";
        case TokenType::OP_MINUS:      return "minus";
        case TokenType::OP_TIMES:      return "times";
        case TokenType::OP_IDIV:       return "idiv";
        case TokenType::OP_RDIV:       return "rdiv";
        case TokenType::OP_MOD:        return "imod";
 
        // Logical
        case TokenType::OP_AND:        return "andsy";
        case TokenType::OP_OR:         return "orsy";
        case TokenType::OP_NOT:        return "notsy";
 
        // Comparison
        case TokenType::EQL:           return "eql";
        case TokenType::NEQ:           return "neq";
        case TokenType::GTR:           return "gtr";
        case TokenType::GEQ:           return "geq";
        case TokenType::LSS:           return "lss";
        case TokenType::LEQ:           return "leq";
 
        // Delimiters
        case TokenType::LPAR:          return "lparent";
        case TokenType::RPAR:          return "rparent";
        case TokenType::LBRACK:        return "lbrack";
        case TokenType::RBRACK:        return "rbrack";
        case TokenType::COMMA:         return "comma";
        case TokenType::SEMICOLON:     return "semicolon";
        case TokenType::PERIOD:        return "period";
        case TokenType::COLON:         return "colon";
        case TokenType::BECOMES:       return "becomes";
 
        // Identifier & comment
        case TokenType::IDENT:         return "ident";
        case TokenType::COMMENT:       return "comment";
 
        // Declaration keywords
        case TokenType::KW_CONST:      return "constsy";
        case TokenType::KW_TYPE:       return "typesy";
        case TokenType::KW_VAR:        return "varsy";
        case TokenType::KW_FUNCTION:   return "functionsy";
        case TokenType::KW_PROCEDURE:  return "proceduresy";
        case TokenType::KW_ARRAY:      return "arraysy";
        case TokenType::KW_RECORD:     return "recordsy";
        case TokenType::KW_PROGRAM:    return "programsy";
 
        // Control flow keywords
        case TokenType::KW_BEGIN:      return "beginsy";
        case TokenType::KW_END:        return "endsy";
        case TokenType::KW_IF:         return "ifsy";
        case TokenType::KW_THEN:       return "thensy";
        case TokenType::KW_ELSE:       return "elsesy";
        case TokenType::KW_WHILE:      return "whilesy";
        case TokenType::KW_DO:         return "dosy";
        case TokenType::KW_FOR:        return "forsy";
        case TokenType::KW_TO:         return "tosy";
        case TokenType::KW_DOWNTO:     return "downtosy";
        case TokenType::KW_REPEAT:     return "repeatsy";
        case TokenType::KW_UNTIL:      return "untilsy";
        case TokenType::KW_CASE:       return "casesy";
        case TokenType::KW_OF:         return "ofsy";
 
        // Special
        case TokenType::KW_EOF:        return "eof";
        case TokenType::UNKNOWN:       return "error";
 
        default:                       return "unknown";
    }
}
 
// To handle tokens that carry unique value with it
string Token::toString() const {
    switch (type) {
        case TokenType::IDENT:
        case TokenType::INTCON:
        case TokenType::REALCON:
        case TokenType::CHARCON:
        case TokenType::STRING:
        case TokenType::COMMENT:
        case TokenType::UNKNOWN:
            return tokenTypeName() + " (" + value + ")";
 
        default:
            return tokenTypeName();
    }
}
