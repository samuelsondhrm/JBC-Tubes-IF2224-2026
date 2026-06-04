#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitProcDecl(ProcDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();

    int paramCount = 0;
    for (ParamNode* param : n->params) {
        paramCount += static_cast<int>(param->names.size());
    }

    int varCount = 0;
    if (n->block) {
        for (ASTNode* decl : n->block->declarations) {
            if (decl->kind == ASTKind::VarDecl) {
                varCount += static_cast<int>(static_cast<VarDeclNode*>(decl)->names.size());
            }
        }
    }

    emit("INT", 0, 3 + paramCount + varCount);

    currentDepth_++;      // masuk ke scope prosedur
    visitNode(n->block);
    currentDepth_--;      // keluar dari scope prosedur

    emit("RET", 0, 0);

    frameStack_.pop_back();
    currentLevel_--;
}

void ICGenerator::visitFuncDecl(FuncDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();

    int paramCount = 0;
    for (ParamNode* param : n->params) {
        paramCount += static_cast<int>(param->names.size());
    }

    int varCount = 0;
    if (n->block) {
        for (ASTNode* decl : n->block->declarations) {
            if (decl->kind == ASTKind::VarDecl) {
                varCount += static_cast<int>(static_cast<VarDeclNode*>(decl)->names.size());
            }
        }
    }

    // TODO (Faza): interpreter RET handler harus preserve nilai puncak eval stack ini.
    emit("INT", 0, 3 + paramCount + varCount);

    currentDepth_++;      // masuk ke scope fungsi
    visitNode(n->block);
    currentDepth_--;      // keluar dari scope fungsi

    emit("RET", 0, 0);
}
