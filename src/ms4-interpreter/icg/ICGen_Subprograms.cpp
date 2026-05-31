#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitProcDecl(ProcDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();
    int varCount = 0;
    if (n->block) {
        for (ASTNode* decl : n->block->declarations) {
            if (decl->kind == ASTKind::VarDecl) {
                varCount += static_cast<VarDeclNode*>(decl)->names.size();
            }
        }
    }
    emit("INT", 0, 3 + varCount);
    visitNode(n->block);
    emit("RET", 0, 0);
}

void ICGenerator::visitFuncDecl(FuncDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();
    int varCount = 0;
    if (n->block) {
        for (ASTNode* decl : n->block->declarations) {
            if (decl->kind == ASTKind::VarDecl) {
                varCount += static_cast<VarDeclNode*>(decl)->names.size();
            }
        }
    }
    emit("INT", 0, 3 + varCount);
    visitNode(n->block);
    emit("RET", 0, 0);
}
