#include "DecoratedASTPrinter.hpp"

void DecoratedASTPrinter::print(ASTNode* root, std::ostream& out) {
    if (!root) return;
    printNode(root, "", true, out);
}

void DecoratedASTPrinter::printSemanticInfo(const SemanticInfo& sem, std::ostream& out) {
    bool first = true;
    out << " \u2192 "; // right arrow
    if (sem.tab_index != -1) { out << "tab:" << sem.tab_index; first = false; }
    if (sem.type_code != 0) { 
        if (!first) out << ", ";
        out << "type:" << sem.type_code; 
        first = false;
    }
    if (sem.lev != 0) {
        if (!first) out << ", ";
        out << "lev:" << sem.lev;
    }
}

void DecoratedASTPrinter::printNode(ASTNode* node, const std::string& prefix, bool isLast, std::ostream& out) {
    if (!node) return;
    out << prefix;
    out << (isLast ? "\u2514\u2500\u2500 " : "\u251C\u2500\u2500 "); // └── or ├──
    
    std::string nextPrefix = prefix + (isLast ? "    " : "\u2502   "); // "    " or "│   "

    // Print Node content
    switch (node->kind) {
        case ASTKind::Program: {
            auto* p = static_cast<ProgramNode*>(node);
            out << "ProgramNode(name: '" << p->name << "')";
            printSemanticInfo(p->sem, out); out << "\n";
            std::vector<ASTNode*> children = p->declarations;
            if (p->body) children.push_back(p->body);
            for (size_t i = 0; i < children.size(); ++i) {
                printNode(children[i], nextPrefix, i == children.size() - 1, out);
            }
            break;
        }
        case ASTKind::VarDecl: {
            auto* v = static_cast<VarDeclNode*>(node);
            out << "VarDecl(";
            for(size_t i=0; i<v->names.size(); ++i) { out << "'" << v->names[i] << "'" << (i+1 < v->names.size() ? ", " : ""); }
            out << ")";
            printSemanticInfo(v->sem, out); out << "\n";
            if (v->typeNode) printNode(v->typeNode, nextPrefix, true, out);
            break;
        }
        case ASTKind::Compound: {
            auto* c = static_cast<CompoundNode*>(node);
            out << "Compound"; printSemanticInfo(c->sem, out); out << "\n";
            for (size_t i = 0; i < c->statements.size(); ++i) {
                printNode(c->statements[i], nextPrefix, i == c->statements.size() - 1, out);
            }
            break;
        }
        case ASTKind::Assign: {
            auto* a = static_cast<AssignNode*>(node);
            out << "Assign"; printSemanticInfo(a->sem, out); out << "\n";
            if (a->target) printNode(a->target, nextPrefix, false, out);
            if (a->value) printNode(a->value, nextPrefix, true, out);
            break;
        }
        case ASTKind::Var: {
            auto* v = static_cast<VarNode*>(node);
            out << "Var('" << v->name << "')"; printSemanticInfo(v->sem, out); out << "\n";
            break;
        }
        case ASTKind::IntLit: {
            auto* i = static_cast<IntLitNode*>(node);
            out << "IntLit(" << i->value << ")"; printSemanticInfo(i->sem, out); out << "\n";
            break;
        }
        case ASTKind::BinOp: {
            auto* b = static_cast<BinOpNode*>(node);
            out << "BinOp('" << b->op << "')"; printSemanticInfo(b->sem, out); out << "\n";
            if (b->left) printNode(b->left, nextPrefix, false, out);
            if (b->right) printNode(b->right, nextPrefix, true, out);
            break;
        }
        // Very simplified, expand later
        default:
            out << "ASTNode(" << (int)node->kind << ")"; printSemanticInfo(node->sem, out); out << "\n";
            break;
    }
}
