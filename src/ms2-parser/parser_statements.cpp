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
    ParseNode* node = new ParseNode("<while-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_WHILE, "while-statement")));
    node->addChild(parseExpression());

    node->addChild(makeTerminal(expect(TokenType::KW_DO, "while-statement")));
    node->addChild(parseStatement());
 
    return node;
};

ParseNode* Parser::parseRepeatStatement(){
    ParseNode* node = new ParseNode("<repeat-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_REPEAT, "repeat-statement")));
    node->addChild(parseStatementList());

    node->addChild(makeTerminal(expect(TokenType::KW_UNTIL, "repeat-statement")));
    node->addChild(parseExpression());
 

    return node;
};


ParseNode* Parser::parseForStatement(){
    ParseNode* node = new ParseNode("<for-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_FOR, "for-statement")));
    node->addChild(makeTerminal(expect(TokenType::IDENT, "for-statement")));
    node->addChild(makeTerminal(expect(TokenType::BECOMES, "for-statement")));
    node->addChild(parseExpression());
 
    if (check(TokenType::KW_TO)) {
        node->addChild(makeTerminal(consume()));
    } else if (check(TokenType::KW_DOWNTO)) {
        node->addChild(makeTerminal(consume()));
    } else {
        error("expected 'to' or 'downto' in for-statement", current());
    }
 
    node->addChild(parseExpression());
    node->addChild(makeTerminal(expect(TokenType::KW_DO, "for-statement")));
    node->addChild(parseStatement());
 
    return node;
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