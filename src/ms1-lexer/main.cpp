#include "token/token.hpp"
#include "util/file.hpp"
#include "lexer/lexer.hpp"
#include <algorithm> 
#include <iostream>

using namespace std; 

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./lexer <input.txt> [output.txt]" << "\n";
        return 1;
    }

    // Read source file
    string src;
    try {
        src = FileUtil::readFile(argv[1]);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // Tokenize
    Lexer lexer(src);
    vector<Token> tokens = lexer.tokenize();

    // Print
    tokens.erase(
            std::remove_if(tokens.begin(), tokens.end(), [](const Token& t){
                return t.type == TokenType::COMMENT;
                }),
            tokens.end()
            );
                
    for (const Token& t : tokens) {
        cout << t.toString() << "\n";
    }


    if (argc >= 3) { 
	    string s; 
	    for (auto const& t : tokens) { 
		s += t.toString() + "\n";
	} 
	    FileUtil::writeFile(argv[2], s);
	    cout << "File saved at " << argv[2] << endl; 
	}

    return 0;
}
