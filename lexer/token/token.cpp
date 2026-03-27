#include "token.hpp" 
#include <iostream>  
using namespace std;


Token::Token(tokenType type, string value, int line) : type(type), value(value), line(line) { 
	cout << "Token constructed" << endl; 
} 
