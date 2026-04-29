#include "Parser.hpp"

ParseNode* Parser::parseStatement() {
};

ParseNode* Parser::parseAssignmentStatement(const Token &identTok){

};
ParseNode* Parser::parseIfStatement(){

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