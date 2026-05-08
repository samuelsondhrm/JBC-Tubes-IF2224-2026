#include "SemanticAnalyzer.hpp"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {
    symTable.initialize();
}

void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;
    visitNode(root);
}

bool SemanticAnalyzer::hasError() const {
    return !errors_.empty();
}

void SemanticAnalyzer::printSymbolTables(std::ostream& out) const {
    // Akan diimplementasikan lengkap saat SymbolTable sudah siap.
    // symTable.print(out);
}

void SemanticAnalyzer::semanticError(const std::string& msg, int line) {
    std::string err = "Semantic error at line " + std::to_string(line) + ": " + msg;
    errors_.push_back(err);
    std::cerr << err << "\n";
}

void SemanticAnalyzer::visitNode(ASTNode* node) {
    if (!node) return;
    switch (node->kind) {
        case ASTKind::Program:    visitProgram(static_cast<ProgramNode*>(node)); break;
        case ASTKind::Block:      visitBlock(static_cast<BlockNode*>(node)); break;
        case ASTKind::VarDecl:    visitVarDecl(static_cast<VarDeclNode*>(node)); break;
        case ASTKind::ConstDecl:  visitConstDecl(static_cast<ConstDeclNode*>(node)); break;
        case ASTKind::TypeDecl:   visitTypeDecl(static_cast<TypeDeclNode*>(node)); break;
        case ASTKind::ProcDecl:   visitProcDecl(static_cast<ProcDeclNode*>(node)); break;
        case ASTKind::FuncDecl:   visitFuncDecl(static_cast<FuncDeclNode*>(node)); break;
        
        case ASTKind::Compound:   visitCompound(static_cast<CompoundNode*>(node)); break;
        case ASTKind::Assign:     visitAssign(static_cast<AssignNode*>(node)); break;
        case ASTKind::If:         visitIf(static_cast<IfNode*>(node)); break;
        case ASTKind::While:      visitWhile(static_cast<WhileNode*>(node)); break;
        case ASTKind::For:        visitFor(static_cast<ForNode*>(node)); break;
        case ASTKind::Repeat:     visitRepeat(static_cast<RepeatNode*>(node)); break;
        case ASTKind::Case:       visitCase(static_cast<CaseNode*>(node)); break;
        case ASTKind::ProcCall:   visitProcCall(static_cast<ProcCallNode*>(node)); break;

        default:
            visitExpr(node);
            break;
    }
}
