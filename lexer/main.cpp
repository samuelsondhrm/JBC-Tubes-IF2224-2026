#include "token/token.hpp" 
#include "util/read.hpp" 
#include "lexer/symbolTable.hpp"
#include <cctype>
#include <iostream> 
#include <sstream>
using namespace std; 

int main() { 
	string readContents; 
	readContents = FileReader::readFile("test.txt"); 

	SymbolTable symTable;
    	TokenType result;
	istringstream stream(readContents);
    	string word;

	while (stream >> word) {
		if (symTable.lookup(word, result)) {
			cout << "KEYWORD : " << word << endl;
		} else {
		    	cout << "IDENT   : " << word << endl;
		}
	}
 
    return 0;
} 
