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
                auto* vd = static_cast<VarDeclNode*>(decl);
                for (const auto& name : vd->names) {
                    int idx = lookupInBlock(name, n->sem.ref);
                    if (idx != -1) {
                        ms3::TabEntry& entry = symTable_.getTabEntry(idx);
                        varCount += symTable_.typeSize(entry.type, entry.ref);
                    } else {
                        varCount += 1;
                    }
                }
            }
        }
    }

    emit("INT", paramCount, 3 + paramCount + varCount);

    FrameInfo f;
    f.paramCount = paramCount;
    f.isFunction = false;
    f.functionName = n->name;
    frameStack_.push_back(f);
    currentLevel_++;

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
                auto* vd = static_cast<VarDeclNode*>(decl);
                for (const auto& name : vd->names) {
                    int idx = lookupInBlock(name, n->sem.ref);
                    if (idx != -1) {
                        ms3::TabEntry& entry = symTable_.getTabEntry(idx);
                        varCount += symTable_.typeSize(entry.type, entry.ref);
                    } else {
                        varCount += 1;
                    }
                }
            }
        }
    }

    // TODO (Faza): interpreter RET handler harus preserve nilai puncak eval stack ini.
    emit("INT", paramCount, 4 + paramCount + varCount);

    FrameInfo f;
    f.paramCount = paramCount;
    f.isFunction = true;
    f.functionName = n->name;
    frameStack_.push_back(f);
    currentLevel_++;

    currentDepth_++;      // masuk ke scope fungsi
    visitNode(n->block);
    currentDepth_--;      // keluar dari scope fungsi

    emit("RET", 0, 1);

    frameStack_.pop_back();
    currentLevel_--;
}
