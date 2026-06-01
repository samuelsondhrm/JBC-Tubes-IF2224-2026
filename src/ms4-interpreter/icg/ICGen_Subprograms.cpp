#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitProcDecl(ProcDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();
    
    int paramCount = countParams(n->params);
    int localVarCount = countLocalVars(n->block);

    emit("INT", 0, 3 + paramCount + localVarCount);

    currentLevel_++;
    frameStack_.push_back({paramCount, false, ""});

    if (n->block) visitBlock(n->block);

    emit("RET", 0, 0);

    frameStack_.pop_back();
    currentLevel_--;
}

void ICGenerator::visitFuncDecl(FuncDeclNode* n) {
    if (!n) return;
    funcLabels_[n->name] = currentLine();
    
    int paramCount = countParams(n->params);
    int localVarCount = countLocalVars(n->block);

    emit("INT", 0, 3 + 1 + paramCount + localVarCount);

    currentLevel_++;
    frameStack_.push_back({paramCount, true, n->name});

    if (n->block) visitBlock(n->block);

    emit("RET", 0, 1);

    frameStack_.pop_back();
    currentLevel_--;
}
