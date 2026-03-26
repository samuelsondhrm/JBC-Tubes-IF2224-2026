# JBC-Tubes-IF2224-2026
 A custom compiler and interpreter for a programming language named Arion

# Things to implement 

| Class | Purpose | Form | 
| ----- | ------- | ---- | 
| rules ( not a class, tapi perlu ) | Creates each states' transition from one to antother | .txt file | 
| DFALoader | Loads from rules.txt and returns a DFA | .cpp | 
| DFA | The DFA per se | .cpp | 
| Transition | Lookups from DFA | .cpp | 
| DFA State | The DFA State | .cpp | 
| Token | Reading every char from file | .cpp |  
| TokenType | Holds map of 52 tokens | .cpp | 
| SymbolTable | Set of keywrods | .cpp | 
| Lexer | Main driver | .cpp | 
