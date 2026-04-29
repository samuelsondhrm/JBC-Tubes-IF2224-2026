#include "Parser.hpp"

ParseNode* Parser::parseStatement() {
};

ParseNode* Parser::parseAssignmentStatement(const Token &identTok){
    ParseNode* node = new ParseNode("<assignment-statement>");
    node->addChild(makeTerminal(identTok));
    node->addChild(makeTerminal(expect(TokenType::BECOMES, "assignment-statement")));
    
    node->addChild(parseExpression());

    return node;
};


ParseNode* Parser::parseIfStatement(){
    ParseNode* node = new ParseNode("<if-statement>");
    node->addChild(makeTerminal(expect(TokenType::KW_IF, "if-statement")));
    node->addChild(parseExpression());

    node->addChild(makeTerminal(expect(TokenType::KW_THEN, "if-statement")));
    node->addChild(parseStatement());
    
    
    if(check(TokenType::KW_ELSE)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseStatement());
    }
    return node;
};

ParseNode* Parser::parseCaseStatement(){
    ParseNode* node = new ParseNode("<case-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_CASE, "case-statement")));
    node->addChild(parseExpression());                                            
    
    node->addChild(makeTerminal(expect(TokenType::KW_OF, "case-statement"))); 
    node->addChild(parseCaseBlock());                     
    node->addChild(makeTerminal(expect(TokenType::KW_END, "case-statement")));
 
    return node;
};

ParseNode* Parser::parseCaseBlock(){
    ParseNode* node = new ParseNode("<case-block>");
 
    node->addChild(parseConstant());
    while (check(TokenType::COMMA)) {
        node->addChild(makeTerminal(consume()));
        node->addChild(parseConstant());
    }
 
    node->addChild(makeTerminal(expect(TokenType::COLON, "case-block")));
    node->addChild(parseStatement());
    while (check(TokenType::SEMICOLON)) {
        node->addChild(makeTerminal(consume()));
 
        
        if (!check(TokenType::KW_END)) {
            node->addChild(parseCaseBlock());
        }
    }
 
    return node;
};


ParseNode* Parser::parseWhileStatement(){

};
ParseNode* Parser::parseRepeatStatement(){

};
ParseNode* Parser::parseForStatement(){

};


ParseNode* Parser::parseProcedureFunctionCall(const Token &identTok){
    ParseNode* node = new ParseNode("<procedure-call>");
    node->addChild(makeTerminal(identTok));
    node->addChild(makeTerminal(expect(TokenType::LPAR, "procedure-call")));

    if(!check(TokenType::RPAR)) {
        node->addChild(parseParameterList());
    }
    node->addChild(makeTerminal(expect(TokenType::RPAR, "procedure-call")));
    
    return node;
};