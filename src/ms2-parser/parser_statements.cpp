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

};
ParseNode* Parser::parseCaseBlock(){

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