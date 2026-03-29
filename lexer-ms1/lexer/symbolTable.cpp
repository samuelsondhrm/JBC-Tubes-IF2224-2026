#include "symbolTable.hpp"
#include <algorithm>
#include <cctype>

using namespace std;

SymbolTable::SymbolTable(){
    keywords["not"]     = TokenType::OP_NOT;
    keywords["div"]     = TokenType::OP_IDIV;
    keywords["mod"]     = TokenType::OP_MOD;
    keywords["and"]     = TokenType::OP_AND;
    keywords["or"]      = TokenType::OP_OR;
    keywords["const"]   = TokenType::KW_CONST;
    keywords["type"]    = TokenType::KW_TYPE;
    keywords["var"]     = TokenType::KW_VAR;
    keywords["function"]= TokenType::KW_FUNCTION;
    keywords["procedure"]= TokenType::KW_PROCEDURE;
    keywords["array"]   = TokenType::KW_ARRAY;
    keywords["record"]  = TokenType::KW_RECORD;
    keywords["program"] = TokenType::KW_PROGRAM;
    keywords["begin"]   = TokenType::KW_BEGIN;
    keywords["end"]     = TokenType::KW_END;
    keywords["if"]      = TokenType::KW_IF;
    keywords["then"]    = TokenType::KW_THEN;
    keywords["else"]    = TokenType::KW_ELSE;
    keywords["while"]   = TokenType::KW_WHILE;
    keywords["do"]      = TokenType::KW_DO;
    keywords["for"]     = TokenType::KW_FOR;
    keywords["to"]      = TokenType::KW_TO;
    keywords["downto"]  = TokenType::KW_DOWNTO;
    keywords["repeat"]  = TokenType::KW_REPEAT;
    keywords["until"]   = TokenType::KW_UNTIL;
    keywords["case"]    = TokenType::KW_CASE;
    keywords["of"]      = TokenType::KW_OF;
};

bool SymbolTable::lookup(const string& word, TokenType& out) const{
    string lower = word;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    auto it = keywords.find(lower);
    if (it != keywords.end()) {
        out = it->second;
        return true;
    }
    return false;
};