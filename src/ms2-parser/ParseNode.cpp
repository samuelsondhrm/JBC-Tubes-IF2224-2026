#include "ParseNode.hpp"

ParseNode::ParseNode(const std::string& name, const std::string& value)
    : name(name), value(value) {}

// Recursively delete all children to prevent memory leaks.
ParseNode::~ParseNode() {
    for (ParseNode* child : children) {
        delete child;
    }
}

void ParseNode::addChild(ParseNode* child) {
    if (child != nullptr) {
        children.push_back(child);
    }
}
