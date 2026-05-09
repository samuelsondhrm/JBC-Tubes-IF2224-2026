#include "ASTBuilder.hpp"
#include <iostream>

static std::string tokenNameToOp(const std::string& name) {
    if (name == "eql")    return "==";
    if (name == "neq")    return "<>";
    if (name == "lss")    return "<";
    if (name == "leq")    return "<=";
    if (name == "gtr")    return ">";
    if (name == "geq")    return ">=";
    if (name == "plus")   return "+";
    if (name == "minus")  return "-";
    if (name == "times")  return "*";
    if (name == "rdiv")   return "/";
    if (name == "idiv")   return "div";
    if (name == "imod")   return "mod";
    if (name == "andsy")  return "and";
    if (name == "orsy")   return "or";
    return name;
}

ASTNode* ASTBuilder::build(ParseNode* node) {
    return buildNode(node);
}

ParseNode* ASTBuilder::findChild(ParseNode* node, const std::string& childName) {
    for (auto* child : node->children) {
        if (child->name == childName) return child;
    }
    return nullptr;
}

std::vector<ParseNode*> ASTBuilder::findChildren(ParseNode* node, const std::string& childName) {
    std::vector<ParseNode*> res;
    for (auto* child : node->children) {
        if (child->name == childName) res.push_back(child);
    }
    return res;
}

ASTNode* ASTBuilder::buildNode(ParseNode* node) {
    if (!node) return nullptr;
    if (node->name == "<program>")              return buildProgram(node);
    if (node->name == "<block>")                return buildBlock(node);
    if (node->name == "<procedure-declaration>")return buildProcDecl(node);
    if (node->name == "<function-declaration>") return buildFuncDecl(node);
    if (node->name == "<compound-statement>")   return buildCompound(node);
    if (node->name == "<assignment-statement>") return buildAssign(node);
    if (node->name == "<if-statement>")         return buildIf(node);
    if (node->name == "<while-statement>")      return buildWhile(node);
    if (node->name == "<for-statement>")        return buildFor(node);
    if (node->name == "<repeat-statement>")     return buildRepeat(node);
    if (node->name == "<case-statement>")       return buildCase(node);
    if (node->name == "<procedure/function-call>") return buildProcCall(node);
    if (node->name == "<statement>")            return buildStatement(node);
    if (node->name == "<expression>")           return buildExpression(node);
    if (node->name == "<simple-expression>")    return buildSimpleExpression(node);
    if (node->name == "<term>")                 return buildTerm(node);
    if (node->name == "<factor>")               return buildFactor(node);
    if (node->name == "<type>")                 return buildType(node);
    if (node->name == "<variable>")             return buildVariable(node);

    if (node->name == "<array-type>")           return buildArrayType(node);
    if (node->name == "<record-type>")          return buildRecordType(node);
    if (node->name == "<range>")                return buildRange(node);
    if (node->name == "<enumerated>")           return buildEnumerated(node);

    return nullptr;
}

ASTNode* ASTBuilder::buildProgram(ParseNode* node) {
    auto* p = new ProgramNode();
    p->kind = ASTKind::Program;
    p->line = node->line;
    
    ParseNode* header = findChild(node, "<program-header>");
    if (header) {
        ParseNode* ident = findChild(header, "ident");
        if (ident) p->name = ident->value;
    }

    ParseNode* declPart = findChild(node, "<declaration-part>");
    if (declPart) buildDeclarationPart(declPart, p->declarations);

    ParseNode* compound = findChild(node, "<compound-statement>");
    if (compound) p->body = buildCompound(compound);

    return p;
}

ASTNode* ASTBuilder::buildBlock(ParseNode* node) {
    auto* b = new BlockNode();
    b->kind = ASTKind::Block;
    b->line = node->line;
    
    ParseNode* declPart = findChild(node, "<declaration-part>");
    if (declPart) buildDeclarationPart(declPart, b->declarations);

    ParseNode* compound = findChild(node, "<compound-statement>");
    if (compound) b->body = buildCompound(compound);

    return b;
}

void ASTBuilder::buildDeclarationPart(ParseNode* node, std::vector<ASTNode*>& decls) {
    for (auto* child : node->children) {
        if (child->name == "<const-declaration>") {
            buildConstDecl(child, decls);
        } else if (child->name == "<type-declaration>") {
            buildTypeDecl(child, decls);
        } else if (child->name == "<var-declaration>") {
            buildVarDecl(child, decls);
        } else if (child->name == "<subprogram-declaration>") {
            if (!child->children.empty()) {
                ASTNode* d = buildNode(child->children[0]);
                if (d) decls.push_back(d);
            }
        }
    }
}

void ASTBuilder::buildVarDecl(ParseNode* node, std::vector<ASTNode*>& decls) {
    auto idLists = findChildren(node, "<identifier-list>");
    auto typeNodes = findChildren(node, "<type>");
    size_t count = std::min(idLists.size(), typeNodes.size());
    for (size_t i = 0; i < count; ++i) {
        auto* v = new VarDeclNode();
        v->kind = ASTKind::VarDecl;
        v->line = idLists[i]->line;
        for (auto* ident : findChildren(idLists[i], "ident")) {
            v->names.push_back(ident->value);
        }
        v->typeNode = buildType(typeNodes[i]);
        decls.push_back(v);
    }
}

void ASTBuilder::buildConstDecl(ParseNode* node, std::vector<ASTNode*>& decls) {
    auto idents = findChildren(node, "ident");
    auto constants = findChildren(node, "<constant>");
    size_t count = std::min(idents.size(), constants.size());
    for (size_t i = 0; i < count; ++i) {
        auto* c = new ConstDeclNode();
        c->kind = ASTKind::ConstDecl;
        c->line = idents[i]->line;
        c->name = idents[i]->value;
        if (!constants[i]->children.empty()) {
            ParseNode* inner = constants[i]->children[0];
            if (inner->name == "intcon") { auto* it = new IntLitNode(); it->kind = ASTKind::IntLit; it->value = std::stoi(inner->value); it->line = inner->line; c->value = it; }
            else if (inner->name == "realcon") { auto* r = new RealLitNode(); r->kind = ASTKind::RealLit; r->value = std::stod(inner->value); r->line = inner->line; c->value = r; }
            else if (inner->name == "charcon") { auto* ch = new CharLitNode(); ch->kind = ASTKind::CharLit; ch->value = inner->value.size() >= 2 ? inner->value[1] : '\0'; ch->line = inner->line; c->value = ch; }
            else if (inner->name == "string") { auto* s = new StringLitNode(); s->kind = ASTKind::StringLit; s->value = inner->value; s->line = inner->line; c->value = s; }
            else if (inner->name == "ident") { auto* v = new VarNode(); v->kind = ASTKind::Var; v->name = inner->value; v->line = inner->line; c->value = v; }
        }
        decls.push_back(c);
    }
}

void ASTBuilder::buildTypeDecl(ParseNode* node, std::vector<ASTNode*>& decls) {
    auto idents = findChildren(node, "ident");
    auto typeNodes = findChildren(node, "<type>");
    size_t count = std::min(idents.size(), typeNodes.size());
    for (size_t i = 0; i < count; ++i) {
        auto* t = new TypeDeclNode();
        t->kind = ASTKind::TypeDecl;
        t->line = idents[i]->line;
        t->name = idents[i]->value;
        t->typeNode = buildType(typeNodes[i]);
        decls.push_back(t);
    }
}

ASTNode* ASTBuilder::buildProcDecl(ParseNode* node) {
    auto* p = new ProcDeclNode();
    p->kind = ASTKind::ProcDecl;
    p->line = node->line;
    
    ParseNode* ident = findChild(node, "ident");
    if (ident) p->name = ident->value;
    
    ParseNode* formals = findChild(node, "<formal-parameter-list>");
    if (formals) {
        for (auto* group : findChildren(formals, "<parameter-group>")) {
            auto* param = new ParamNode();
            param->kind = ASTKind::Param;
            param->line = group->line;
            param->isByRef = (findChild(group, "varsy") != nullptr);
            
            ParseNode* idList = findChild(group, "<identifier-list>");
            if (idList) {
                for (auto* id : findChildren(idList, "ident")) {
                    param->names.push_back(id->value);
                }
            }
            ParseNode* t = findChild(group, "<type>");
            if (t) {
                param->typeNode = buildType(t);
            } else {
                for (size_t j = 0; j < group->children.size(); ++j) {
                    if (group->children[j]->name == "colon" && j+1 < group->children.size()) {
                        ParseNode* ti = group->children[j+1];
                        if (ti->name == "ident") {
                            auto* s = new SimpleTypeNode();
                            s->kind = ASTKind::SimpleType; s->typeName = ti->value; s->line = ti->line;
                            param->typeNode = s;
                        }
                        break;
                    }
                }
            }

            p->params.push_back(param);
        }
    }
    ParseNode* block = findChild(node, "<block>");
    if (block) p->block = static_cast<BlockNode*>(buildBlock(block));
    return p;
}

ASTNode* ASTBuilder::buildFuncDecl(ParseNode* node) {
    auto* f = new FuncDeclNode();
    f->kind = ASTKind::FuncDecl;
    f->line = node->line;
    
    ParseNode* ident = findChild(node, "ident");
    if (ident) f->name = ident->value;
    
    ParseNode* formals = findChild(node, "<formal-parameter-list>");
    if (formals) {
        for (auto* group : findChildren(formals, "<parameter-group>")) {
            auto* param = new ParamNode();
            param->kind = ASTKind::Param;
            param->line = group->line;
            param->isByRef = (findChild(group, "varsy") != nullptr);
            
            ParseNode* idList = findChild(group, "<identifier-list>");
            if (idList) {
                for (auto* id : findChildren(idList, "ident")) {
                    param->names.push_back(id->value);
                }
            }
            ParseNode* t = findChild(group, "<type>");
            if (t) {
                param->typeNode = buildType(t);
            } else {
                for (size_t j = 0; j < group->children.size(); ++j) {
                    if (group->children[j]->name == "colon" && j+1 < group->children.size()) {
                        ParseNode* ti = group->children[j+1];
                        if (ti->name == "ident") {
                            auto* s = new SimpleTypeNode();
                            s->kind = ASTKind::SimpleType; s->typeName = ti->value; s->line = ti->line;
                            param->typeNode = s;
                        }
                        break;
                    }
                }
            }
            f->params.push_back(param);
        }
    }
    
    // Find return type
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (node->children[i]->name == "colon" && i + 1 < node->children.size() && node->children[i+1]->name == "ident") {
            f->returnTypeName = node->children[i+1]->value;
            break;
        }
    }
    
    ParseNode* block = findChild(node, "<block>");
    if (block) f->block = static_cast<BlockNode*>(buildBlock(block));
    return f;
}

ASTNode* ASTBuilder::buildType(ParseNode* node) {
    if (node->children.empty()) return nullptr;
    ParseNode* child = node->children[0];
    if (child->name == "ident") {
        auto* s = new SimpleTypeNode();
        s->kind = ASTKind::SimpleType;
        s->typeName = child->value;
        s->line = child->line;
        return s;
    } else {
        return buildNode(child);
    }
}

ASTNode* ASTBuilder::buildArrayType(ParseNode* node) {
    auto* a = new ArrayTypeNode();
    a->kind = ASTKind::ArrayType;
    a->line = node->line;
    
    ParseNode* indexType = findChild(node, "<range>");
    if (!indexType) indexType = findChild(node, "ident"); // could be simple type
    
    if (indexType) {
        if (indexType->name == "ident") {
             auto* s = new SimpleTypeNode(); s->kind = ASTKind::SimpleType; s->typeName = indexType->value; s->line = indexType->line;
             a->indexType = s;
        } else {
             a->indexType = buildRange(indexType);
        }
    }
    
    ParseNode* typeNode = findChild(node, "<type>");
    if (typeNode) a->elementType = buildType(typeNode);
    return a;
}

ASTNode* ASTBuilder::buildRecordType(ParseNode* node) {
    auto* r = new RecordTypeNode();
    r->kind = ASTKind::RecordType;
    r->line = node->line;
    
    ParseNode* fieldList = findChild(node, "<field-list>");
    if (fieldList) {
        for (auto* fieldPart : findChildren(fieldList, "<field-part>")) {
            auto* fd = new FieldDeclNode();
            fd->kind = ASTKind::FieldDecl;
            fd->line = fieldPart->line;
            ParseNode* idList = findChild(fieldPart, "<identifier-list>");
            if (idList) {
                for (auto* id : findChildren(idList, "ident")) {
                    fd->names.push_back(id->value);
                }
            }
            ParseNode* t = findChild(fieldPart, "<type>");
            if (t) fd->typeNode = buildType(t);
            r->fields.push_back(fd);
        }
    }
    return r;
}

ASTNode* ASTBuilder::buildRange(ParseNode* node) {
    auto* r = new RangeNode();
    r->kind = ASTKind::Range;
    r->line = node->line;
    
    auto consts = findChildren(node, "<constant>");
    if (consts.size() == 2) {
        // Mock literal construction
        auto makeLit = [](ParseNode* cn) -> ASTNode* {
            if (cn->children.empty()) return nullptr;
            ParseNode* inner = cn->children[0];
            if (inner->name == "intcon") { auto* i = new IntLitNode(); i->kind = ASTKind::IntLit; i->value = std::stoi(inner->value); return i; }
            if (inner->name == "ident") { auto* v = new VarNode(); v->kind = ASTKind::Var; v->name = inner->value; return v; }
            return nullptr;
        };
        r->low = makeLit(consts[0]);
        r->high = makeLit(consts[1]);
    }
    return r;
}

ASTNode* ASTBuilder::buildEnumerated(ParseNode* node) {
    auto* e = new EnumeratedNode();
    e->kind = ASTKind::Enumerated;
    e->line = node->line;
    
    for (auto* id : findChildren(node, "ident")) {
        e->values.push_back(id->value);
    }
    return e;
}

ASTNode* ASTBuilder::buildStatement(ParseNode* node) {
    if (node->children.empty()) return nullptr;
    return buildNode(node->children[0]);
}

ASTNode* ASTBuilder::buildCompound(ParseNode* node) {
    auto* c = new CompoundNode();
    c->kind = ASTKind::Compound;
    c->line = node->line;
    
    ParseNode* stmtList = findChild(node, "<statement-list>");
    if (stmtList) {
        for (auto* stmt : findChildren(stmtList, "<statement>")) {
            ASTNode* st = buildStatement(stmt);
            if (st) c->statements.push_back(st);
        }
    }
    return c;
}

ASTNode* ASTBuilder::buildAssign(ParseNode* node) {
    auto* a = new AssignNode();
    a->kind = ASTKind::Assign;
    a->line = node->line;
    
    a->target = buildVariable(node->children[0]);
    ParseNode* expr = findChild(node, "<expression>");
    if (expr) a->value = buildExpression(expr);
    
    return a;
}

ASTNode* ASTBuilder::buildIf(ParseNode* node) {
    auto* i = new IfNode();
    i->kind = ASTKind::If;
    i->line = node->line;
    
    ParseNode* exprNode = findChild(node, "<expression>");
    if (exprNode) i->condition = buildExpression(exprNode);
    auto stmts = findChildren(node, "<statement>");
    if (stmts.size() > 0) i->thenBranch = buildStatement(stmts[0]);
    if (stmts.size() > 1) i->elseBranch = buildStatement(stmts[1]);
    
    return i;
}

ASTNode* ASTBuilder::buildWhile(ParseNode* node) {
    auto* w = new WhileNode();
    w->kind = ASTKind::While;
    w->line = node->line;
    
    ParseNode* exprNode = findChild(node, "<expression>");
    if (exprNode) w->condition = buildExpression(exprNode);
    ParseNode* bodyNode = findChild(node, "<statement>");
    if (bodyNode) w->body = buildStatement(bodyNode);
    
    return w;
}

ASTNode* ASTBuilder::buildFor(ParseNode* node) {
    auto* f = new ForNode();
    f->kind = ASTKind::For;
    f->line = node->line;
    
    ParseNode* ident = findChild(node, "ident");
    if (ident) f->varName = ident->value;
    
    auto exprs = findChildren(node, "<expression>");
    if (exprs.size() > 0) f->fromExpr = buildExpression(exprs[0]);
    if (exprs.size() > 1) f->toExpr = buildExpression(exprs[1]);
    
    if (findChild(node, "downtosy")) f->downto = true;
    
    ParseNode* bodyNode = findChild(node, "<statement>");
    if (bodyNode) f->body = buildStatement(bodyNode);

    return f;
}

ASTNode* ASTBuilder::buildRepeat(ParseNode* node) {
    auto* r = new RepeatNode();
    r->kind = ASTKind::Repeat;
    r->line = node->line;
    
    ParseNode* stmtList = findChild(node, "<statement-list>");
    if (stmtList) {
        for (auto* stmt : findChildren(stmtList, "<statement>")) {
            ASTNode* st = buildStatement(stmt);
            if (st) r->body.push_back(st);
        }
    }
    ParseNode* exprNode = findChild(node, "<expression>");
    if (exprNode) r->condition = buildExpression(exprNode);
    return r;
}

static void flattenCaseBlocks(ParseNode* block, std::vector<ParseNode*>& out) {
    if (!block || block->name != "<case-block>") return;
    out.push_back(block);
    for (auto* child : block->children) {
        if (child->name == "<case-block>") flattenCaseBlocks(child, out);
    }
}

ASTNode* ASTBuilder::buildCase(ParseNode* node) {
    auto* c = new CaseNode();
    c->kind = ASTKind::Case;
    c->line = node->line;
    
    ParseNode* exprNode = findChild(node, "<expression>");
    if (exprNode) c->expr = buildExpression(exprNode);
    
    std::vector<ParseNode*> allBlocks;
    ParseNode* firstBlock = findChild(node, "<case-block>");
    flattenCaseBlocks(firstBlock, allBlocks);
    
    for (auto* block : allBlocks) {
        auto* branch = new CaseBranchNode();
        branch->kind = ASTKind::CaseBranch;
        branch->line = block->line;
        
        for (auto* cnst : findChildren(block, "<constant>")) {
            // Very simplified
            if (!cnst->children.empty()) {
                ParseNode* inner = cnst->children[0];
                if (inner->name == "intcon") { auto* i = new IntLitNode(); i->kind = ASTKind::IntLit; i->value = std::stoi(inner->value); branch->labels.push_back(i); }
                else if (inner->name == "ident") { auto* v = new VarNode(); v->kind = ASTKind::Var; v->name = inner->value; branch->labels.push_back(v); }
            }
        }
        branch->statement = buildStatement(findChild(block, "<statement>"));
        c->branches.push_back(branch);
    }
    return c;
}

ASTNode* ASTBuilder::buildProcCall(ParseNode* node) {
    auto* p = new ProcCallNode();
    p->kind = ASTKind::ProcCall;
    p->line = node->line;
    
    ParseNode* ident = findChild(node, "ident");
    if (ident) p->name = ident->value;
    
    ParseNode* params = findChild(node, "<parameter-list>");
    if (params) {
        for (auto* expr : findChildren(params, "<expression>")) {
            p->args.push_back(buildExpression(expr));
        }
    }
    return p;
}

ASTNode* ASTBuilder::buildFuncCall(ParseNode* node) {
    auto* f = new FuncCallNode();
    f->kind = ASTKind::FuncCall;
    f->line = node->line;
    
    ParseNode* ident = findChild(node, "ident");
    if (ident) f->name = ident->value;
    
    ParseNode* params = findChild(node, "<parameter-list>");
    if (params) {
        for (auto* expr : findChildren(params, "<expression>")) {
            f->args.push_back(buildExpression(expr));
        }
    }
    return f;
}

// Flat tree generation for binary expressions
ASTNode* ASTBuilder::buildExpression(ParseNode* node) {
    auto& children = node->children;
    ASTNode* left = nullptr;
    
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i]->name == "<simple-expression>") {
            if (!left) {
                left = buildSimpleExpression(children[i]);
            } else {
                std::string op = tokenNameToOp(children[i-1]->children[0]->name);
                auto* bin = new BinOpNode();
                bin->kind = ASTKind::BinOp;
                bin->line = children[i-1]->line;
                bin->op = op;
                bin->left = left;
                bin->right = buildSimpleExpression(children[i]);
                left = bin;
            }
        }
    }
    return left;
}

ASTNode* ASTBuilder::buildSimpleExpression(ParseNode* node) {
    auto& children = node->children;
    ASTNode* left = nullptr;
    
    size_t i = 0;
    if (i < children.size() && (children[i]->name == "plus" || children[i]->name == "minus")) {
        auto* un = new UnaryOpNode();
        un->kind = ASTKind::UnaryOp;
        un->line = children[i]->line;
        un->op = tokenNameToOp(children[i]->name);
        i++;
        if (i < children.size() && children[i]->name == "<term>") {
            un->operand = buildTerm(children[i]);
            left = un;
            i++;
        }
    }
    
    for (; i < children.size(); i++) {
        if (children[i]->name == "<term>") {
            if (!left) {
                left = buildTerm(children[i]);
            } else {
                std::string op = tokenNameToOp(children[i-1]->children[0]->name);
                auto* bin = new BinOpNode();
                bin->kind = ASTKind::BinOp;
                bin->line = children[i-1]->line;
                bin->op = op;
                bin->left = left;
                bin->right = buildTerm(children[i]);
                left = bin;
            }
        }
    }
    return left;
}

ASTNode* ASTBuilder::buildTerm(ParseNode* node) {
    auto& children = node->children;
    ASTNode* left = nullptr;
    
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i]->name == "<factor>") {
            if (!left) {
                left = buildFactor(children[i]);
            } else {
                std::string op = tokenNameToOp(children[i-1]->children[0]->name);
                auto* bin = new BinOpNode();
                bin->kind = ASTKind::BinOp;
                bin->line = children[i-1]->line;
                bin->op = op;
                bin->left = left;
                bin->right = buildFactor(children[i]);
                left = bin;
            }
        }
    }
    return left;
}

ASTNode* ASTBuilder::buildFactor(ParseNode* node) {
    if (node->children.empty()) return nullptr;
    ParseNode* child = node->children[0];
    
    if (child->name == "<procedure/function-call>") return buildFuncCall(child);
    if (child->name == "<variable>")                return buildVariable(child);
    if (child->name == "intcon")  { auto* i = new IntLitNode(); i->kind = ASTKind::IntLit; i->value = std::stoi(child->value); i->line = child->line; return i; }
    if (child->name == "realcon") { auto* r = new RealLitNode(); r->kind = ASTKind::RealLit; r->value = std::stod(child->value); r->line = child->line; return r; }
    if (child->name == "charcon") { auto* c = new CharLitNode(); c->kind = ASTKind::CharLit; c->value = child->value.size()>=2 ? child->value[1] : '\0'; c->line = child->line; return c; }
    if (child->name == "string")  { auto* s = new StringLitNode(); s->kind = ASTKind::StringLit; s->value = child->value; s->line = child->line; return s; }
    if (child->name == "boolcon") { auto* b = new BoolLitNode(); b->kind = ASTKind::BoolLit; b->value = (child->value == "true"); b->line = child->line; return b; }
    if (child->name == "notsy")   { auto* u = new UnaryOpNode(); u->kind = ASTKind::UnaryOp; u->op = "not"; u->line = child->line; u->operand = buildFactor(node->children[1]); return u; }
    if (child->name == "lparent") return buildExpression(node->children[1]);
    
    return nullptr;
}

ASTNode* ASTBuilder::buildVariable(ParseNode* node) {
    ParseNode* ident = findChild(node, "ident");
    if (!ident) return nullptr;
    
    ASTNode* base = new VarNode();
    base->kind = ASTKind::Var;
    static_cast<VarNode*>(base)->name = ident->value;
    base->line = ident->line;
    
    for (auto* comp : findChildren(node, "<component-variable>")) {
        if (comp->children.empty()) continue;
        if (comp->children[0]->name == "lbrack") {
            ParseNode* idxList = findChild(comp, "<index-list>");
            if (idxList) {
                // assume single index for now
                if (idxList && !idxList->children.empty()) {
                    ParseNode* idxTok = idxList->children[0];  // intcon, charcon, atau ident
                    ASTNode* idxNode = nullptr;
                    if (idxTok->name == "intcon") {
                        auto* i = new IntLitNode(); i->kind = ASTKind::IntLit;
                        i->value = std::stoi(idxTok->value); i->line = idxTok->line;
                        idxNode = i;
                    } else if (idxTok->name == "ident") {
                        auto* v = new VarNode(); v->kind = ASTKind::Var;
                        v->name = idxTok->value; v->line = idxTok->line;
                        idxNode = v;
                    } else if (idxTok->name == "charcon") {
                        auto* c = new CharLitNode(); c->kind = ASTKind::CharLit;
                        c->value = idxTok->value.size() >= 2 ? idxTok->value[1] : '\0'; c->line = idxTok->line;
                        idxNode = c;
                    }
                    if (idxNode) {
                        auto* arr = new ArrayAccessNode();
                        arr->kind = ASTKind::ArrayAccess; arr->line = comp->line;
                        arr->array = base; arr->index = idxNode;
                        base = arr;
                    }
                }
            }
        } else if (comp->children[0]->name == "period") {
            ParseNode* field = findChild(comp, "ident");
            if (field) {
                auto* rec = new RecordAccessNode();
                rec->kind = ASTKind::RecordAccess;
                rec->line = comp->line;
                rec->record = base;
                rec->field = field->value;
                base = rec;
            }
        }
    }
    return base;
}
