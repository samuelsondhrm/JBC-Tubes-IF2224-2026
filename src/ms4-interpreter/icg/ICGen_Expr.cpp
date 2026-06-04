#include "ICGenerator.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <stdexcept>

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
    if (n->sem.tab_index < 0) {
        throw std::runtime_error(
            "ICGen Error: VarNode '" + n->name +
            "' memiliki tab_index tidak valid -- tidak dianotasi oleh SemanticAnalyzer");
    }
    ms3::TabEntry& entry = symTable_.getTabEntry(n->sem.tab_index);
    
    emit("LOD", runtimeLevel(entry), runtimeAddr(entry));
}

void ICGenerator::visitAssign(AssignNode* n) {
    if (n->target->kind == ASTKind::ArrayAccess) {
        emitAddr(n->target);
        visitNode(n->value);
        emit("STOI", 0, 0);
    } else if (n->target->kind == ASTKind::Var) {
        visitNode(n->value);
        auto* varNode = static_cast<VarNode*>(n->target);
        if (varNode->sem.tab_index < 0) {
            throw std::runtime_error(
                "ICGen Error: assignment target '" + varNode->name +
                "' memiliki tab_index tidak valid yang tidak dianotasi oleh SemanticAnalyzer");
        }
        ms3::TabEntry& entry = symTable_.getTabEntry(varNode->sem.tab_index);
        if (entry.obj == OBJ_FUNCTION) {
            emit("STO", 0, 3);
        } else {
            emit("STO", runtimeLevel(entry), runtimeAddr(entry));
        }
    }
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
        {"and", 4},   // AND via MUL: benar untuk boolean (0*x=0, 1*1=1)
    };

    if (n->op == "or") {
        emit("OPR", 0, 2); // ADD
        emit("LIT", 0, 0);
        emit("OPR", 0, 8); // NEQ: 1 jika (sum != 0), 0 jika (sum == 0)
        return;
    }

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
        emit("CAL", -static_cast<int>(n->args.size()) - 1, it->second);
    } else {
        throw std::runtime_error(
            "ICGen Error: fungsi '" + n->name + "' tidak ditemukan di funcLabels_");
    }
}

void ICGenerator::emitAddr(ASTNode* node) {
    if (node->kind == ASTKind::Var) {
        auto* varNode = static_cast<VarNode*>(node);
        if (varNode->sem.tab_index < 0) {
            throw std::runtime_error("ICGen Error: VarNode '" + varNode->name + "' has invalid tab_index");
        }
        ms3::TabEntry& entry = symTable_.getTabEntry(varNode->sem.tab_index);
        emit("LODA", runtimeLevel(entry), runtimeAddr(entry));
    } else if (node->kind == ASTKind::ArrayAccess) {
        auto* arrNode = static_cast<ArrayAccessNode*>(node);
        emitAddr(arrNode->array);
        visitNode(arrNode->index);
        int atabIdx = arrNode->array->sem.ref;
        ms3::AtabEntry& a = symTable_.getAtabEntry(atabIdx);
        emit("IXA", a.low, a.high);
    } else {
        throw std::runtime_error("ICGen Error: L-value not supported for address generation");
    }
}

void ICGenerator::visitArrayAccess(ArrayAccessNode* n) {
    emitAddr(n);
    emit("LODI", 0, 0);
}

void ICGenerator::visitRecordAccess(RecordAccessNode* n) {
    (void)n;
    throw std::runtime_error(
        "ICGen Error: akses field record belum diimplementasikan (fitur bonus M4)");
}
