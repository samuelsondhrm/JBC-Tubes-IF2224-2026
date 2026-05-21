#include "SemanticAnalyzer.hpp"
#include <string>
#include <algorithm>
#include <vector>

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

    // Check target is writable (not a constant)
    if (node->target && node->target->sem.tab_index >= 0)
    {
        ms3::TabEntry &t = symTable.getTabEntry(node->target->sem.tab_index);
        if (t.obj == OBJ_CONSTANT)
        {
            semanticError("Cannot assign to constant '" + t.name + "'", node->line);
        }
    }

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

void SemanticAnalyzer::visitFor(ForNode *node)
{
    int idx = symTable.lookup(node->varName);
    if (idx == -1)
    {
        semanticError("Undeclared identifier: '" + node->varName + "'", node->line);
    }
    else
    {
        ms3::TabEntry &e = symTable.getTabEntry(idx);
        if (e.type != TYPE_INTEGER)
        {
            semanticError("For loop variable must be Integer type", node->line);
        }
    }

    visitExpr(node->fromExpr);
    int fromType = node->fromExpr ? node->fromExpr->sem.type_code : TYPE_NONE;
    if (!typeChecker.isCompatible(fromType, TYPE_INTEGER))
    {
        semanticError("For loop lower bound must be Integer", node->line);
    }

    visitExpr(node->toExpr);
    int toType = node->toExpr ? node->toExpr->sem.type_code : TYPE_NONE;
    if (!typeChecker.isCompatible(toType, TYPE_INTEGER))
    {
        semanticError("For loop upper bound must be Integer", node->line);
    }

    visitNode(node->body);
}

void SemanticAnalyzer::visitRepeat(RepeatNode *node)
{
    for (ASTNode *stmt : node->body)
    {
        visitNode(stmt);
    }

    visitExpr(node->condition);
    int condType = node->condition ? node->condition->sem.type_code : TYPE_NONE;
    if (condType != TYPE_BOOLEAN)
    {
        semanticError("Repeat condition must be Boolean, got " + typeName(condType), node->line);
    }
}

void SemanticAnalyzer::visitCase(CaseNode *node)
{
    visitExpr(node->expr);
    int caseType = node->expr ? node->expr->sem.type_code : TYPE_NONE;

    if (caseType != TYPE_INTEGER && caseType != TYPE_CHAR && caseType != TYPE_BOOLEAN)
    {
        semanticError("Case expression must be Integer, Char, or Boolean", node->line);
    }

    for (CaseBranchNode *branch : node->branches)
    {
        for (ASTNode *label : branch->labels)
        {
            visitExpr(label);
            int labelType = label ? label->sem.type_code : TYPE_NONE;
            if (!typeChecker.isCompatible(labelType, caseType))
            {
                semanticError("Case label type mismatch", label ? label->line : node->line);
            }
        }
        visitNode(branch->statement);
    }
}


void SemanticAnalyzer::visitProcCall(ProcCallNode *node)
{
    std::string lowerName = node->name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });

    if (lowerName == "writeln" || lowerName == "write" || lowerName == "readln" || lowerName == "read")
    {
        for (ASTNode *arg : node->args)
        {
            visitExpr(arg);
        }
        node->sem.type_code = TYPE_VOID;
        return;
    }

    int idx = symTable.lookup(node->name);
    if (idx == -1)
    {
        semanticError("Undeclared identifier: '" + node->name + "'", node->line);
        node->sem.type_code = TYPE_VOID;
        return;
    }

    ms3::TabEntry &e = symTable.getTabEntry(idx);
    if (e.obj != OBJ_PROCEDURE)
    {
        semanticError("'" + node->name + "' is not a procedure", node->line);
        node->sem.type_code = TYPE_VOID;
        return;
    }

    ms3::BtabEntry &b = symTable.getBtabEntry(e.ref);
    int expectedArgs = 0;
    {
        int cur = b.lpar;
        while (cur >= 1) {
            ms3::TabEntry& te = symTable.getTabEntry(cur);
            if (te.lev <= e.lev) break;
            expectedArgs++;
            if (te.link == cur - 1) { cur = te.link; }
            else break;
        }
    }
    int actualArgs = static_cast<int>(node->args.size());

    if (actualArgs != expectedArgs)
    {
        semanticError("Wrong number of arguments for '" + node->name + "': expected " + std::to_string(expectedArgs) + ", got " + std::to_string(actualArgs), node->line);
    }

    std::vector<int> paramIndices;
    {
        int cur = b.lpar;
        while (cur != -1 && cur >= 0 && static_cast<int>(paramIndices.size()) < expectedArgs)
        {
            paramIndices.push_back(cur);
            cur = symTable.getTabEntry(cur).link;
        }
        std::reverse(paramIndices.begin(), paramIndices.end()); // urut dari parameter pertama
    }

    int checkCount = std::min(actualArgs, (int)paramIndices.size());

    for (int i = 0; i < static_cast<int>(node->args.size()); ++i)
    {
        visitExpr(node->args[i]);

        if (i < checkCount)
        {
            ms3::TabEntry &param = symTable.getTabEntry(paramIndices[i]);
            int argType = node->args[i] ? node->args[i]->sem.type_code : TYPE_NONE;
            int paramType = param.type;

            if (!typeChecker.isCompatible(argType, paramType))
            {
                semanticError("Type mismatch for argument " + std::to_string(i + 1) + " of '" + node->name + "'", node->line);
            }
        }
    }

    node->sem.type_code = TYPE_VOID;
    node->sem.tab_index = idx;
}
