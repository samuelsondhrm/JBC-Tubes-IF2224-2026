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
    
    int relLev = currentDepth_ - entry.lev;
    emit("LOD", relLev, entry.adr + 3);
}

void ICGenerator::visitAssign(AssignNode* n) {
    visitNode(n->value);

    if (n->target->kind == ASTKind::Var) {
        auto* varNode = static_cast<VarNode*>(n->target);
        if (varNode->sem.tab_index < 0) {
            throw std::runtime_error(
                "ICGen Error: assignment target '" + varNode->name +
                "' memiliki tab_index tidak valid yang tidak dianotasi oleh SemanticAnalyzer");
        }
        ms3::TabEntry& entry = symTable_.getTabEntry(varNode->sem.tab_index);
        if (entry.obj == OBJ_FUNCTION) {
            // TODO (Faza): handler RET di interpreter.cpp harus simpan nilai puncak eval stack
            // sebelum restoreFrame(), lalu push kembali setelah frame pulih, sehingga caller
            // menerima return value di atas eval stack-nya setelah instruksi CAL selesai.
        } else {
            int relLev = currentDepth_ - entry.lev;
            emit("STO", relLev, entry.adr + 3);
        }
    }
    // Assignment ke ArrayAccess/RecordAccess tidak dalam scope M4 dasar (fitur bonus).
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
    // TODO (Faza): handler RET di interpreter.cpp harus:
    //   1. Simpan nilai puncak eval stack (return value) sebelum restoreFrame().
    //   2. Push kembali nilai tersebut setelah frame dipulihkan.
    // Ini memastikan caller menerima return value di atas eval stack-nya pasca-CAL.
    for (ASTNode* arg : n->args) {
        visitNode(arg);
    }

    auto it = funcLabels_.find(n->name);
    if (it != funcLabels_.end()) {
        emit("CAL", 0, it->second);
    } else {
        throw std::runtime_error(
            "ICGen Error: fungsi '" + n->name + "' tidak ditemukan di funcLabels_");
    }
}

void ICGenerator::visitArrayAccess(ArrayAccessNode* n) {
    (void)n;
    throw std::runtime_error(
        "ICGen Error: akses array belum diimplementasikan (fitur bonus M4)");
}

void ICGenerator::visitRecordAccess(RecordAccessNode* n) {
    (void)n;
    throw std::runtime_error(
        "ICGen Error: akses field record belum diimplementasikan (fitur bonus M4)");
}
