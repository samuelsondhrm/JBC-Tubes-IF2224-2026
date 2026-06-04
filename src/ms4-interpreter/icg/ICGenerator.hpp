#pragma once
#include "ICInstruction.hpp"
#include "../../ms3-semantic/ast/ASTNode.hpp"
#include "../../ms3-semantic/symtable/SymbolTable.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class ICGenerator {
public:
    ICGenerator(ms3::SymbolTable& symTable);

    std::vector<ICInstruction> generate(ASTNode* root);
    void writeToFile(const std::vector<ICInstruction>& code, const std::string& filename);

private:
    ms3::SymbolTable& symTable_;
    std::vector<ICInstruction> code_;
    int nextLine_ = 0;
    int currentDepth_ = 0;
    std::unordered_map<std::string, int> funcLabels_;

    struct FrameInfo {
        int paramCount = 0;
        bool isFunction = false;
        std::string functionName;
    };

    int currentLevel_ = 0;
    std::vector<FrameInfo> frameStack_;

    int runtimeLevel(const ms3::TabEntry& entry) const;
    int runtimeAddr(const ms3::TabEntry& entry) const;
    int countParams(const std::vector<ParamNode*>& params) const;
    int countLocalVars(BlockNode* block) const;

    void emit(const std::string& op, int level, int arg);
    int  currentLine() const;
    void backpatch(int targetLine, int value);
    void visitNode(ASTNode* node);

    void visitProgram(ProgramNode* n);
    void visitBlock(BlockNode* n);

    // ICGen_Expr.cpp
    void visitAssign(AssignNode* n);
    void visitBinOp(BinOpNode* n);
    void visitUnaryOp(UnaryOpNode* n);
    void visitVar(VarNode* n);
    void visitArrayAccess(ArrayAccessNode* n);
    void visitRecordAccess(RecordAccessNode* n);
    void visitIntLit(IntLitNode* n);
    void visitRealLit(RealLitNode* n);
    void visitCharLit(CharLitNode* n);
    void visitStringLit(StringLitNode* n);
    void visitBoolLit(BoolLitNode* n);
    void visitFuncCall(FuncCallNode* n);

    // ICGen_Statements.cpp
    void visitCompound(CompoundNode* n);
    void visitIf(IfNode* n);
    void visitWhile(WhileNode* n);
    void visitFor(ForNode* n);
    void visitRepeat(RepeatNode* n);
    void visitCase(CaseNode* n);
    void visitProcCall(ProcCallNode* n);

    // ICGen_Subprograms.cpp 
    void visitProcDecl(ProcDeclNode* n);
    void visitFuncDecl(FuncDeclNode* n);
};
