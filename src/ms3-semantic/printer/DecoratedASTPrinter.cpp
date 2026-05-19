#include "DecoratedASTPrinter.hpp"

void DecoratedASTPrinter::print(ASTNode* root, std::ostream& out) {
    if (!root) return;
    printNode(root, "", true, out);
}

void DecoratedASTPrinter::printSemanticInfo(const SemanticInfo& sem, std::ostream& out) {
    if (sem.tab_index == -1 && sem.type_code == 0 && sem.lev == 0) return;
 
    out << " \u2192 "; // →
    bool first = true;
    if (sem.tab_index != -1) {
        out << "tab:" << sem.tab_index;
        first = false;
    }
    if (sem.type_code != 0) {
        if (!first) out << ", ";
        out << "type:" << sem.type_code;
        first = false;
    }
    if (sem.lev != 0) {
        if (!first) out << ", ";
        out << "lev:" << sem.lev;
    }
    if (sem.ref != 0) {
        if (!first) out << ", ";
        out << "ref:" << sem.ref;
    }
}


void DecoratedASTPrinter::printNode(ASTNode* node, const std::string& prefix, bool isLast, std::ostream& out) {
    if (!node) return;
 
    out << prefix << (isLast ? "\u2514\u2500\u2500 " : "\u251C\u2500\u2500 ");
    std::string next = prefix + (isLast ? "    " : "\u2502   ");
 
    switch (node->kind) {
 
    // ── PROGRAM LEVEL ─────────────────────────
 
    case ASTKind::Program: {
        auto* n = static_cast<ProgramNode*>(node);
        out << "Program('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        std::vector<ASTNode*> ch(n->declarations.begin(), n->declarations.end());
        if (n->body) ch.push_back(n->body);
        for (size_t i = 0; i < ch.size(); ++i)
            printNode(ch[i], next, i == ch.size() - 1, out);
        break;
    }
 
    case ASTKind::Block: {
        auto* n = static_cast<BlockNode*>(node);
        out << "Block";
        printSemanticInfo(n->sem, out); out << "\n";
        std::vector<ASTNode*> ch(n->declarations.begin(), n->declarations.end());
        if (n->body) ch.push_back(n->body);
        for (size_t i = 0; i < ch.size(); ++i)
            printNode(ch[i], next, i == ch.size() - 1, out);
        break;
    }
 
    // ── DECLARATIONS ──────────────────────────
 
    case ASTKind::VarDecl: {
        auto* n = static_cast<VarDeclNode*>(node);
        out << "VarDecl(";
        for (size_t i = 0; i < n->names.size(); ++i)
            out << "'" << n->names[i] << "'" << (i + 1 < n->names.size() ? ", " : "");
        out << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->typeNode) printNode(n->typeNode, next, true, out);
        break;
    }
 
    case ASTKind::ConstDecl: {
        auto* n = static_cast<ConstDeclNode*>(node);
        out << "ConstDecl('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->value) printNode(n->value, next, true, out);
        break;
    }
 
    case ASTKind::TypeDecl: {
        auto* n = static_cast<TypeDeclNode*>(node);
        out << "TypeDecl('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->typeNode) printNode(n->typeNode, next, true, out);
        break;
    }
 
    case ASTKind::ProcDecl: {
        auto* n = static_cast<ProcDeclNode*>(node);
        out << "ProcDecl('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        size_t total = n->params.size() + (n->block ? 1 : 0);
        size_t i = 0;
        for (ParamNode* p : n->params) {
            printNode(p, next, ++i == total, out);
        }
        if (n->block) printNode(n->block, next, true, out);
        break;
    }
 
    case ASTKind::FuncDecl: {
        auto* n = static_cast<FuncDeclNode*>(node);
        out << "FuncDecl('" << n->name << "', returns: " << n->returnTypeName << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        size_t total = n->params.size() + (n->block ? 1 : 0);
        size_t i = 0;
        for (ParamNode* p : n->params) {
            printNode(p, next, ++i == total, out);
        }
        if (n->block) printNode(n->block, next, true, out);
        break;
    }
 
    case ASTKind::Param: {
        auto* n = static_cast<ParamNode*>(node);
        out << "Param(" << (n->isByRef ? "var " : "");
        for (size_t i = 0; i < n->names.size(); ++i)
            out << "'" << n->names[i] << "'" << (i + 1 < n->names.size() ? ", " : "");
        out << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->typeNode) printNode(n->typeNode, next, true, out);
        break;
    }
 
    // ── TYPE NODES ────────────────────────────
 
    case ASTKind::SimpleType: {
        auto* n = static_cast<SimpleTypeNode*>(node);
        out << "SimpleType('" << n->typeName << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::ArrayType: {
        auto* n = static_cast<ArrayTypeNode*>(node);
        out << "ArrayType";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->indexType)   printNode(n->indexType,   next, !n->elementType, out);
        if (n->elementType) printNode(n->elementType, next, true, out);
        break;
    }
 
    case ASTKind::RecordType: {
        auto* n = static_cast<RecordTypeNode*>(node);
        out << "RecordType";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->fields.size(); ++i)
            printNode(n->fields[i], next, i == n->fields.size() - 1, out);
        break;
    }
 
    case ASTKind::Range: {
        auto* n = static_cast<RangeNode*>(node);
        out << "Range";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->low)  printNode(n->low,  next, !n->high, out);
        if (n->high) printNode(n->high, next, true, out);
        break;
    }
 
    case ASTKind::Enumerated: {
        auto* n = static_cast<EnumeratedNode*>(node);
        out << "Enumerated(";
        for (size_t i = 0; i < n->values.size(); ++i)
            out << "'" << n->values[i] << "'" << (i + 1 < n->values.size() ? ", " : "");
        out << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        // No AST children — values are plain strings
        break;
    }
 
    case ASTKind::FieldDecl: {
        auto* n = static_cast<FieldDeclNode*>(node);
        out << "FieldDecl(";
        for (size_t i = 0; i < n->names.size(); ++i)
            out << "'" << n->names[i] << "'" << (i + 1 < n->names.size() ? ", " : "");
        out << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->typeNode) printNode(n->typeNode, next, true, out);
        break;
    }
 
    // ── STATEMENTS ────────────────────────────
 
    case ASTKind::Compound: {
        auto* n = static_cast<CompoundNode*>(node);
        out << "Compound";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->statements.size(); ++i)
            printNode(n->statements[i], next, i == n->statements.size() - 1, out);
        break;
    }
 
    case ASTKind::Assign: {
        auto* n = static_cast<AssignNode*>(node);
        out << "Assign";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->target) printNode(n->target, next, !n->value, out);
        if (n->value)  printNode(n->value,  next, true, out);
        break;
    }
 
    case ASTKind::If: {
        auto* n = static_cast<IfNode*>(node);
        out << "If";
        printSemanticInfo(n->sem, out); out << "\n";
        bool hasElse = n->elseBranch != nullptr;
        if (n->condition)  printNode(n->condition,  next, !n->thenBranch && !hasElse, out);
        if (n->thenBranch) printNode(n->thenBranch, next, !hasElse, out);
        if (n->elseBranch) printNode(n->elseBranch, next, true, out);
        break;
    }
 
    case ASTKind::While: {
        auto* n = static_cast<WhileNode*>(node);
        out << "While";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->condition) printNode(n->condition, next, !n->body, out);
        if (n->body)      printNode(n->body,      next, true, out);
        break;
    }
 
    case ASTKind::For: {
        auto* n = static_cast<ForNode*>(node);
        out << "For('" << n->varName << "', " << (n->downto ? "downto" : "to") << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->fromExpr) printNode(n->fromExpr, next, !n->toExpr && !n->body, out);
        if (n->toExpr)   printNode(n->toExpr,   next, !n->body, out);
        if (n->body)     printNode(n->body,      next, true, out);
        break;
    }
 
    case ASTKind::Repeat: {
        auto* n = static_cast<RepeatNode*>(node);
        out << "Repeat";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->body.size(); ++i)
            printNode(n->body[i], next, !n->condition && i == n->body.size() - 1, out);
        if (n->condition) printNode(n->condition, next, true, out);
        break;
    }
 
    case ASTKind::Case: {
        auto* n = static_cast<CaseNode*>(node);
        out << "Case";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->expr) printNode(n->expr, next, n->branches.empty(), out);
        for (size_t i = 0; i < n->branches.size(); ++i)
            printNode(n->branches[i], next, i == n->branches.size() - 1, out);
        break;
    }
 
    case ASTKind::CaseBranch: {
        auto* n = static_cast<CaseBranchNode*>(node);
        out << "CaseBranch";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->labels.size(); ++i)
            printNode(n->labels[i], next, !n->statement && i == n->labels.size() - 1, out);
        if (n->statement) printNode(n->statement, next, true, out);
        break;
    }
 
    case ASTKind::ProcCall: {
        auto* n = static_cast<ProcCallNode*>(node);
        out << "ProcCall('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->args.size(); ++i)
            printNode(n->args[i], next, i == n->args.size() - 1, out);
        break;
    }
 
    // ── EXPRESSIONS ───────────────────────────
 
    case ASTKind::BinOp: {
        auto* n = static_cast<BinOpNode*>(node);
        out << "BinOp('" << n->op << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->left)  printNode(n->left,  next, !n->right, out);
        if (n->right) printNode(n->right, next, true, out);
        break;
    }
 
    case ASTKind::UnaryOp: {
        auto* n = static_cast<UnaryOpNode*>(node);
        out << "UnaryOp('" << n->op << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->operand) printNode(n->operand, next, true, out);
        break;
    }
 
    case ASTKind::Var: {
        auto* n = static_cast<VarNode*>(node);
        out << "Var('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::ArrayAccess: {
        auto* n = static_cast<ArrayAccessNode*>(node);
        out << "ArrayAccess";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->array) printNode(n->array, next, !n->index, out);
        if (n->index) printNode(n->index, next, true, out);
        break;
    }
 
    case ASTKind::RecordAccess: {
        auto* n = static_cast<RecordAccessNode*>(node);
        out << "RecordAccess('" << n->field << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        if (n->record) printNode(n->record, next, true, out);
        break;
    }
 
    case ASTKind::IntLit: {
        auto* n = static_cast<IntLitNode*>(node);
        out << "IntLit(" << n->value << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::RealLit: {
        auto* n = static_cast<RealLitNode*>(node);
        out << "RealLit(" << n->value << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::CharLit: {
        auto* n = static_cast<CharLitNode*>(node);
        out << "CharLit('" << n->value << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::StringLit: {
        auto* n = static_cast<StringLitNode*>(node);
        out << "StringLit(" << n->value << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::BoolLit: {
        auto* n = static_cast<BoolLitNode*>(node);
        out << "BoolLit(" << (n->value ? "true" : "false") << ")";
        printSemanticInfo(n->sem, out); out << "\n";
        break;
    }
 
    case ASTKind::FuncCall: {
        auto* n = static_cast<FuncCallNode*>(node);
        out << "FuncCall('" << n->name << "')";
        printSemanticInfo(n->sem, out); out << "\n";
        for (size_t i = 0; i < n->args.size(); ++i)
            printNode(n->args[i], next, i == n->args.size() - 1, out);
        break;
    }
 
    } // end switch
}

