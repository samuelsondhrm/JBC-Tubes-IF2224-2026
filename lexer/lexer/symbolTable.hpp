#pragma once
#include <string>
#include <unordered_map>
#include "token/token.hpp"

using namespace std;

class SymbolTable {
    public: 
        SymbolTable();
        bool lookup(const string& word, TokenType& out) const;
    
    private: 
        unordered_map<string, TokenType> keywords;
};