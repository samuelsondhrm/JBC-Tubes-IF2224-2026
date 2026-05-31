#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitCompound(CompoundNode*) {
    throw std::runtime_error("visitCompound: not implemented");
}

void ICGenerator::visitIf(IfNode*) {
    throw std::runtime_error("visitIf: not implemented");
}

void ICGenerator::visitWhile(WhileNode*) {
    throw std::runtime_error("visitWhile: not implemented");
}

void ICGenerator::visitFor(ForNode*) {
    throw std::runtime_error("visitFor: not implemented");
}

void ICGenerator::visitRepeat(RepeatNode*) {
    throw std::runtime_error("visitRepeat: not implemented");
}

void ICGenerator::visitCase(CaseNode*) {
    throw std::runtime_error("visitCase: not implemented");
}

void ICGenerator::visitProcCall(ProcCallNode*) {
    throw std::runtime_error("visitProcCall: not implemented");
}
