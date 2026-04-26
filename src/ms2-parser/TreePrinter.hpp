#pragma once
#include <ostream>
#include "ParseNode.hpp"

// Prints the parse tree to any output stream (cout, ofstream, etc.)
// using the spec's required format with ├── └── │ connectors.
//
// Usage:
//   printTree(rootNode, std::cout);
//   printTree(rootNode, outputFileStream);
void printTree(const ParseNode* node, std::ostream& out);

// Internal helper — do not call directly.
void printTreeHelper(
    const ParseNode* node,
    const std::string& prefix,
    bool isLast,
    std::ostream& out
);
