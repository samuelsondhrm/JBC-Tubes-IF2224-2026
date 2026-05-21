#include "ParseNode.hpp"
#include "../ms1-lexer/token/token.hpp" 


ParseNode::ParseNode(const std::string& name, const std::string& value)
    : name(name), value(value), isTerminal(false) {}

ParseNode::~ParseNode() {
    for (ParseNode* child : children) {
        delete child;
    }
}

void ParseNode::addChild(ParseNode* child) {
    if (child != nullptr) {
        children.push_back(child);
        if (line == 0 && child->line > 0) {
            line = child->line;
        }
    }
}

inline ParseNode* makeInternal(const std::string& name) { 
    return new ParseNode(name); 
} 

inline ParseNode* makeTerminal(const Token& tok) { 
    ParseNode* node = new ParseNode(tok.tokenTypeName(), tok.value); 
    node->isTerminal = true; 
    return node; 
} 
