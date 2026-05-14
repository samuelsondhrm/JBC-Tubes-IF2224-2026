#include "SemanticAnalyzer.hpp"
#include <string>
#include <algorithm>

void SemanticAnalyzer::visitExpr(ASTNode *node)
{
    if (!node)
        return;
    switch (node->kind)
    {
    case ASTKind::BinOp:
        visitBinOp(static_cast<BinOpNode *>(node));
        break;
    case ASTKind::UnaryOp:
        visitUnaryOp(static_cast<UnaryOpNode *>(node));
        break;
    case ASTKind::Var:
        visitVar(static_cast<VarNode *>(node));
        break;
    case ASTKind::ArrayAccess:
        visitArrayAccess(static_cast<ArrayAccessNode *>(node));
        break;
    case ASTKind::RecordAccess:
        visitRecordAccess(static_cast<RecordAccessNode *>(node));
        break;
    case ASTKind::FuncCall:
        visitFuncCall(static_cast<FuncCallNode *>(node));
        break;
    case ASTKind::IntLit:
        visitIntLit(static_cast<IntLitNode *>(node));
        break;
    case ASTKind::RealLit:
        visitRealLit(static_cast<RealLitNode *>(node));
        break;
    case ASTKind::CharLit:
        visitCharLit(static_cast<CharLitNode *>(node));
        break;
    case ASTKind::StringLit:
        visitStringLit(static_cast<StringLitNode *>(node));
        break;
    case ASTKind::BoolLit:
        visitBoolLit(static_cast<BoolLitNode *>(node));
        break;
    default:
        break;
    }
}

void SemanticAnalyzer::visitIntLit(IntLitNode *node)
{
    node->sem.type_code = TYPE_INTEGER;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitRealLit(RealLitNode *node)
{
    node->sem.type_code = TYPE_REAL;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitCharLit(CharLitNode *node)
{
    node->sem.type_code = TYPE_CHAR;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitStringLit(StringLitNode *node)
{
    node->sem.type_code = TYPE_STRING;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitBoolLit(BoolLitNode *node)
{
    node->sem.type_code = TYPE_BOOLEAN;
    node->sem.lev = currentLevel_;
}



void SemanticAnalyzer::visitVar(VarNode *node)
{
    int idx = symTable.lookup(node->name);
    if (idx == -1)
    {
        semanticError("Undeclared identifier: '" + node->name + "'", node->line);
        node->sem.type_code = TYPE_NONE;
        return;
    }

    ms3::TabEntry &e = symTable.getTabEntry(idx);

    if (e.obj != OBJ_VARIABLE && e.obj != OBJ_CONSTANT && e.obj != OBJ_FUNCTION)
    {
        semanticError("'" + node->name + "' is not a variable or value", node->line);
        node->sem.type_code = TYPE_NONE;
        return;
    }

    node->sem.type_code = e.type;
    node->sem.tab_index = idx;
    node->sem.lev = e.lev;
    node->sem.ref = e.ref;
}


void SemanticAnalyzer::visitBinOp(BinOpNode *node)
{
    visitExpr(node->left);
    visitExpr(node->right);

    int leftType = node->left ? node->left->sem.type_code : TYPE_NONE;
    int rightType = node->right ? node->right->sem.type_code : TYPE_NONE;
    int resultType = typeChecker.getOperatorResultType(node->op, leftType, rightType);

    if (resultType == TYPE_NONE)
    {
        semanticError("Type mismatch for operator '" + node->op + "': incompatible operands", node->line);
    }

    node->sem.type_code = resultType;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitUnaryOp(UnaryOpNode *node)
{
    visitExpr(node->operand);

    int t = node->operand ? node->operand->sem.type_code : TYPE_NONE;

    if (node->op == "not")
    {
        if (t != TYPE_BOOLEAN)
        {
            semanticError("Type mismatch for operator 'not': operand must be Boolean", node->line);
            node->sem.type_code = TYPE_NONE;
            return;
        }
        node->sem.type_code = TYPE_BOOLEAN;
    }
    else if (node->op == "-" || node->op == "+")
    {
        if (t != TYPE_INTEGER && t != TYPE_REAL)
        {
            semanticError("Type mismatch for operator '" + node->op + "': operand must be Integer or Real", node->line);
            node->sem.type_code = TYPE_NONE;
            return;
        }
        node->sem.type_code = t;
    }
    else
    {
        semanticError("Unknown unary operator '" + node->op + "'", node->line);
        node->sem.type_code = TYPE_NONE;
    }
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitArrayAccess(ArrayAccessNode *node){}
void SemanticAnalyzer::visitRecordAccess(RecordAccessNode *node){}
void SemanticAnalyzer::visitFuncCall(FuncCallNode *node){}
