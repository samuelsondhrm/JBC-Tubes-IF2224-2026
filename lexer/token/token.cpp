#include "token.hpp" 
#include <iostream>  
using namespace std;


Token::Token(TokenType type, string value, int line) : type(type), value(value), line(line) { 
	cout << "Token constructed" << endl; 
} 
