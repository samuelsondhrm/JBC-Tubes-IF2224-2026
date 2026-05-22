#pragma once
#include "../ast/ASTNode.hpp"
#include "../symtable/SymbolTable.hpp"
#include "../type/TypeChecker.hpp"
#include <vector>
#include <string>
#include <iostream>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    ~SemanticAnalyzer() = default;

    void analyze(ASTNode* root);
    bool hasError() const;
    void printSymbolTables(std::ostream& out) const;
    void visitProgram(ProgramNode* node);
    void visitBlock(BlockNode* node);
    void visitVarDecl(VarDeclNode* node);
    void visitConstDecl(ConstDeclNode* node);
    void visitTypeDecl(TypeDeclNode* node);
    void visitProcDecl(ProcDeclNode* node);
    void visitFuncDecl(FuncDeclNode* node);
    void visitCompound(CompoundNode* node);
    void visitAssign(AssignNode* node);
    void visitIf(IfNode* node);
    void visitWhile(WhileNode* node);
    void visitFor(ForNode* node);
    void visitRepeat(RepeatNode* node);
    void visitCase(CaseNode* node);
    void visitProcCall(ProcCallNode* node);
    void visitExpr(ASTNode* node);
    void visitBinOp(BinOpNode* node);
    void visitUnaryOp(UnaryOpNode* node);
    void visitVar(VarNode* node);
    void visitArrayAccess(ArrayAccessNode* node);
    void visitRecordAccess(RecordAccessNode* node);
    void visitFuncCall(FuncCallNode* node);
    void visitIntLit(IntLitNode* node);
    void visitRealLit(RealLitNode* node);
    void visitCharLit(CharLitNode* node);
    void visitStringLit(StringLitNode* node);
    void visitBoolLit(BoolLitNode* node);

private:
    ms3::SymbolTable symTable;
    TypeChecker typeChecker;
    std::vector<std::string> errors_;
    int currentLevel_ = 0;

    void semanticError(const std::string& msg, int line);
    void visitNode(ASTNode* node);
};
