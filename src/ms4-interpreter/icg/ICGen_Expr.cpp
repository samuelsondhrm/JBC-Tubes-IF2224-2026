#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitAssign(AssignNode*) {
    throw std::runtime_error("visitAssign: not implemented");
}

void ICGenerator::visitBinOp(BinOpNode*) {
    throw std::runtime_error("visitBinOp: not implemented");
}

void ICGenerator::visitUnaryOp(UnaryOpNode*) {
    throw std::runtime_error("visitUnaryOp: not implemented");
}

void ICGenerator::visitVar(VarNode*) {
    throw std::runtime_error("visitVar: not implemented");
}

void ICGenerator::visitArrayAccess(ArrayAccessNode*) {
    throw std::runtime_error("visitArrayAccess: not implemented");
}

void ICGenerator::visitRecordAccess(RecordAccessNode*) {
    throw std::runtime_error("visitRecordAccess: not implemented");
}

void ICGenerator::visitIntLit(IntLitNode*) {
    throw std::runtime_error("visitIntLit: not implemented");
}

void ICGenerator::visitRealLit(RealLitNode*) {
    throw std::runtime_error("visitRealLit: not implemented");
}

void ICGenerator::visitCharLit(CharLitNode*) {
    throw std::runtime_error("visitCharLit: not implemented");
}

void ICGenerator::visitStringLit(StringLitNode*) {
    throw std::runtime_error("visitStringLit: not implemented");
}

void ICGenerator::visitBoolLit(BoolLitNode*) {
    throw std::runtime_error("visitBoolLit: not implemented");
}

void ICGenerator::visitFuncCall(FuncCallNode*) {
    throw std::runtime_error("visitFuncCall: not implemented");
}
