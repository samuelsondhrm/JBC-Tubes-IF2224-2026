#include "ICGenerator.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>

void ICGenerator::visitIntLit(IntLitNode* n) {
    emit("LIT", 0, n->value);
}

void ICGenerator::visitRealLit(RealLitNode* n) {
    emit("LIT", 0, static_cast<int>(n->value));
}
void ICGenerator::visitCharLit(CharLitNode* n) {
    emit("LIT", 0, static_cast<int>(n->value));
}

void ICGenerator::visitBoolLit(BoolLitNode* n) {
    emit("LIT", 0, n->value ? 1 : 0);
}

void ICGenerator::visitStringLit(StringLitNode* n) {
    (void)n;
    emit("LIT", 0, 0);
}

void ICGenerator::visitVar(VarNode* n) {
    ms3::TabEntry& entry = symTable_.getTabEntry(n->sem.tab_index);
    emit("LOD", runtimeLevel(entry), runtimeAddr(entry));
}

void ICGenerator::visitAssign(AssignNode* n) {
    visitNode(n->value);

    if (n->target->kind == ASTKind::Var) {
        auto* varNode = static_cast<VarNode*>(n->target);
        // assignment return function
        if (!frameStack_.empty() && frameStack_.back().isFunction && varNode->name == frameStack_.back().functionName) {
            emit("STO", 0, 3);
            return;
        }
        ms3::TabEntry& entry = symTable_.getTabEntry(varNode->sem.tab_index);
        emit("STO", runtimeLevel(entry), runtimeAddr(entry));
    }
    // ArrayAccess dan RecordAccess perlu diimplementasi disini kalau jadi ambil bonus
}

void ICGenerator::visitBinOp(BinOpNode* n) {
    visitNode(n->left);
    visitNode(n->right);

    static const std::unordered_map<std::string, int> opMap = {
        {"+",   2},   // ADD
        {"-",   3},   // SUB
        {"*",   4},   // MUL
        {"/",   5},   // DIV (real division)
        {"div", 5},   // DIV (integer division)
        {"mod", 6},   // MOD
        {"=",   7},   // EQL (Arion menggunakan '=' untuk perbandingan)
        {"==",  7},   // EQL (alternatif)
        {"<>",  8},   // NEQ
        {"<",   9},   // LSS
        {">=", 10},   // GEQ
        {">",  11},   // GTR
        {"<=", 12},   // LEQ
        {"and", 4},   // AND — gunakan MUL (1*1=1, 0*x=0)
        {"or",  2},   // OR  — gunakan ADD (clamp di interpreter)
    };

    auto it = opMap.find(n->op);
    if (it != opMap.end()) {
        emit("OPR", 0, it->second);
    }
}

void ICGenerator::visitUnaryOp(UnaryOpNode* n) {
    visitNode(n->operand);

    if (n->op == "-") {
        emit("OPR", 0, 1);
    } else if (n->op == "not") {
        emit("LIT", 0, 0);
        emit("OPR", 0, 7);
    }
}

void ICGenerator::visitFuncCall(FuncCallNode* n) {
    for (ASTNode* arg : n->args) {
        visitNode(arg);
    }

    auto it = funcLabels_.find(n->name);
    if (it != funcLabels_.end()) {
        int argCount = static_cast<int>(n->args.size());
        // function tanpa argumen tetap bisa dibedakan
        emit("CAL", -(argCount + 1), it->second);
    }
}

void ICGenerator::visitArrayAccess(ArrayAccessNode* n) {
    (void)n;
}

void ICGenerator::visitRecordAccess(RecordAccessNode* n) {
    (void)n;
}
