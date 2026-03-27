// #include "token/token.hpp" 
#include "util/read.hpp" 
#include <cctype>
#include <iostream> 
using namespace std; 
int main() { 
	string readContents; 
	readContents = FileReader::readFile("test.txt"); 

	for (auto c : readContents) { 	
		if (isspace(c)) continue; 
		cout << c << endl; 
		
	}
	return 0; 
} 
