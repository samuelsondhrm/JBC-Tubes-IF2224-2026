#include "ICGenerator.hpp"
#include <stdexcept>

void ICGenerator::visitProcDecl(ProcDeclNode*) {
    throw std::runtime_error("visitProcDecl: not implemented");
}

void ICGenerator::visitFuncDecl(FuncDeclNode*) {
    throw std::runtime_error("visitFuncDecl: not implemented");
}
