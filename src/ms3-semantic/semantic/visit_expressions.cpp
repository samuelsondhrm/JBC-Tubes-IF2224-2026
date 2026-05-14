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


void SemanticAnalyzer::visitVar(VarNode *node){}
void SemanticAnalyzer::visitBinOp(BinOpNode *node){}
void SemanticAnalyzer::visitUnaryOp(UnaryOpNode *node){}
void SemanticAnalyzer::visitArrayAccess(ArrayAccessNode *node){}
void SemanticAnalyzer::visitRecordAccess(RecordAccessNode *node){}
void SemanticAnalyzer::visitFuncCall(FuncCallNode *node){}
