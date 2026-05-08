#pragma once
#include "../ast/ASTNode.hpp"
#include "../../ms2-parser/ParseNode.hpp"
#include <vector>
#include <string>

class ASTBuilder {
public:
    ASTNode* build(ParseNode* node);

private:
    ASTNode* buildNode(ParseNode* node);

    ASTNode* buildProgram(ParseNode* node);
    ASTNode* buildBlock(ParseNode* node);
    void buildDeclarationPart(ParseNode* node, std::vector<ASTNode*>& decls);
    ASTNode* buildVarDecl(ParseNode* node);
    ASTNode* buildConstDecl(ParseNode* node);
    ASTNode* buildTypeDecl(ParseNode* node);
    ASTNode* buildProcDecl(ParseNode* node);
    ASTNode* buildFuncDecl(ParseNode* node);
    
    ASTNode* buildType(ParseNode* node);
    ASTNode* buildArrayType(ParseNode* node);
    ASTNode* buildRecordType(ParseNode* node);
    ASTNode* buildRange(ParseNode* node);
    ASTNode* buildEnumerated(ParseNode* node);
    
    ASTNode* buildStatement(ParseNode* node);
    ASTNode* buildAssign(ParseNode* node);
    ASTNode* buildIf(ParseNode* node);
    ASTNode* buildWhile(ParseNode* node);
    ASTNode* buildFor(ParseNode* node);
    ASTNode* buildRepeat(ParseNode* node);
    ASTNode* buildCase(ParseNode* node);
    ASTNode* buildProcCall(ParseNode* node);
    ASTNode* buildCompound(ParseNode* node);
    
    ASTNode* buildExpression(ParseNode* node);
    ASTNode* buildSimpleExpression(ParseNode* node);
    ASTNode* buildTerm(ParseNode* node);
    ASTNode* buildFactor(ParseNode* node);
    
    ASTNode* buildVariable(ParseNode* node);
    ASTNode* buildFuncCall(ParseNode* node);

    ParseNode* findChild(ParseNode* node, const std::string& childName);
    std::vector<ParseNode*> findChildren(ParseNode* node, const std::string& childName);
};
