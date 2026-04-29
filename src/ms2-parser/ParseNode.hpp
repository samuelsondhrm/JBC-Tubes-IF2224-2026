#pragma once
#include <string>
#include <vector>

// Represents a single node in the Arion Parse Tree.
//
// Non-terminal nodes: name = "<program>",  value = ""
// Terminal nodes:     name = "ident",      value = "Hello"   (carries value)
//                     name = "beginsy",    value = ""         (no value)
//
// Display format:
//   non-terminal → <program>
//   terminal w/ value → ident(Hello)
//   terminal w/o value → beginsy
struct ParseNode {
    std::string             name;
    std::string             value;
    bool                    isTerminal; 
    std::vector<ParseNode*> children;

    ParseNode(const std::string& name, const std::string& value = "");
    ~ParseNode();                    // recursively deletes all children
    void addChild(ParseNode* child); // silently ignores nullptr
};
