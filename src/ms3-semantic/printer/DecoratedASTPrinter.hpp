#pragma once
#include "../ast/ASTNode.hpp"
#include <iostream>
#include <string>

class DecoratedASTPrinter {
public:
    void print(ASTNode* root, std::ostream& out);

private:
    void printNode(ASTNode* node, const std::string& prefix, bool isLast, std::ostream& out);
    void printSemanticInfo(const SemanticInfo& sem, std::ostream& out);
};
