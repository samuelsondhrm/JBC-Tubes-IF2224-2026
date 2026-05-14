#include "SemanticAnalyzer.hpp"
#include <string>
#include <algorithm>

static std::string typeName(int tc)
{
    switch (tc)
    {
    case TYPE_INTEGER:
        return "Integer";
    case TYPE_REAL:
        return "Real";
    case TYPE_BOOLEAN:
        return "Boolean";
    case TYPE_CHAR:
        return "Char";
    case TYPE_STRING:
        return "String";
    case TYPE_ARRAY:
        return "Array";
    case TYPE_RECORD:
        return "Record";
    case TYPE_VOID:
        return "Void";
    default:
        return "Unknown";
    }
}

void SemanticAnalyzer::visitCompound(CompoundNode *node)
{
    for (ASTNode *stmt : node->statements)
    {
        visitNode(stmt);
    }
}

void SemanticAnalyzer::visitAssign(AssignNode *node)
{
    visitExpr(node->target);
    visitExpr(node->value);

    int targetType = node->target ? node->target->sem.type_code : TYPE_NONE;
    int valueType = node->value ? node->value->sem.type_code : TYPE_NONE;

    if (!typeChecker.isAssignmentCompatible(targetType, valueType))
    {
        semanticError("Type mismatch in assignment: cannot assign " + typeName(valueType) + " to " + typeName(targetType), node->line);
    }

    node->sem.type_code = TYPE_VOID;
}

void SemanticAnalyzer::visitIf(IfNode *node)
{
    visitExpr(node->condition);

    int condType = node->condition ? node->condition->sem.type_code : TYPE_NONE;
    if (condType != TYPE_BOOLEAN)
    {
        semanticError("If condition must be Boolean, got " + typeName(condType), node->line);
    }

    visitNode(node->thenBranch);

    if (node->elseBranch)
    {
        visitNode(node->elseBranch);
    }
}


void SemanticAnalyzer::visitWhile(WhileNode *node)
{
    visitExpr(node->condition);

    int condType = node->condition ? node->condition->sem.type_code : TYPE_NONE;
    if (condType != TYPE_BOOLEAN)
    {
        semanticError("While condition must be Boolean, got " + typeName(condType), node->line);
    }

    visitNode(node->body);
}

void SemanticAnalyzer::visitFor(ForNode *node){}

void SemanticAnalyzer::visitRepeat(RepeatNode *node){}

void SemanticAnalyzer::visitCase(CaseNode *node){}

void SemanticAnalyzer::visitProcCall(ProcCallNode *node){}
