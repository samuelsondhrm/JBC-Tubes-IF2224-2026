#include "TreePrinter.hpp"

// Prints the root label, then delegates children to printTreeHelper.
void printTree(const ParseNode* node, std::ostream& out) {
    if (!node) return;

    // Root has no connector prefix
    if (node->value.empty()) {
        out << node->name << "\n";
    } else {
        out << node->name << "(" << node->value << ")\n";
    }

    for (size_t i = 0; i < node->children.size(); i++) {
        bool isLast = (i == node->children.size() - 1);
        printTreeHelper(node->children[i], "", isLast, out);
    }
}

void printTreeHelper(
    const ParseNode* node,
    const std::string& prefix,
    bool isLast,
    std::ostream& out
) {
    if (!node) return;

    // "└── " for the last child, "├── " for all others
    out << prefix << (isLast ? "└── " : "├── ");

    // Print the node label
    if (node->value.empty()) {
        out << node->name << "\n";
    } else {
        out << node->name << "(" << node->value << ")\n";
    }

    // Children of the last node are indented with spaces;
    // children of non-last nodes are indented with a vertical bar.
    std::string childPrefix = prefix + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < node->children.size(); i++) {
        bool childIsLast = (i == node->children.size() - 1);
        printTreeHelper(node->children[i], childPrefix, childIsLast, out);
    }
}
