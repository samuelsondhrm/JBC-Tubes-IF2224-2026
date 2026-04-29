#include "Parser.hpp"

//dispatches to the correct statement parser based on the leading token
ParseNode* Parser::parseStatement() {
    ParseNode* node = new ParseNode("statement");
    if (check(TokenType::IDENT)) {
        Token identTok = consume();
        if (check(TokenType::BECOMES)) {
            node->addChild(parseAssignmentStatement(identTok));
        } else if (check(TokenType::LPAR)) {
            node->addChild(parseProcedureFunctionCall(identTok));
        } else {
            ParseNode* callNode = new ParseNode("<procedure-all>");
            callNode->addChild(makeTerminal(identTok));
            node->addChild(callNode);
        }
    } else if (check(TokenType::KW_BEGIN)){
        node->addChild(parseCompoundStatement());
    } else if (check(TokenType::KW_IF)) {
        node->addChild(parseIfStatement());
    } else if (check(TokenType::KW_CASE)) {
        node->addChild(parseCaseStatement());
    } else if (check(TokenType::KW_WHILE)) {
        node->addChild(parseWhileStatement());
    } else if (check(TokenType::KW_REPEAT)) {
        node->addChild(parseRepeatStatement());
    } else {
        delete node;
        return nullptr;
    }
    return node;
};

//parses ident := expression given the pre-consumed ident token
ParseNode* Parser::parseAssignmentStatement(const Token &identTok){
    ParseNode* node = new ParseNode("<assignment-statement>");
    node->addChild(makeTerminal(identTok));
    node->addChild(makeTerminal(expect(TokenType::BECOMES, "assignment-statement")));
    
    node->addChild(parseExpression());

    return node;
};

// parses if expr then statement [else statement]
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

//parses case expression of case-block end, delegating arms to parseCaseBlock
ParseNode* Parser::parseCaseStatement(){
    ParseNode* node = new ParseNode("<case-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_CASE, "case-statement")));
    node->addChild(parseExpression());                                            
    
    node->addChild(makeTerminal(expect(TokenType::KW_OF, "case-statement"))); 
    node->addChild(parseCaseBlock());                     
    node->addChild(makeTerminal(expect(TokenType::KW_END, "case-statement")));
 
    return node;
};
// parses one or more label(s) : statement,  arms recursively until "end" is seen
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

// parses while expr do statement (condition checked before each iteration)
ParseNode* Parser::parseWhileStatement(){
    ParseNode* node = new ParseNode("<while-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_WHILE, "while-statement")));
    node->addChild(parseExpression());

    node->addChild(makeTerminal(expect(TokenType::KW_DO, "while-statement")));
    node->addChild(parseStatement());
 
    return node;
};

// parses repeat statement-list until expression (body runs at least once)
ParseNode* Parser::parseRepeatStatement(){
    ParseNode* node = new ParseNode("<repeat-statement>");
 
    node->addChild(makeTerminal(expect(TokenType::KW_REPEAT, "repeat-statement")));
    node->addChild(parseStatementList());

    node->addChild(makeTerminal(expect(TokenType::KW_UNTIL, "repeat-statement")));
    node->addChild(parseExpression());
 

    return node;
};

// parses for ident := expression to/downto expression do statement with direction check
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

//parses ident([args]) given the pre-consumed ident token
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