#include "ICGenerator.hpp"
#include <stdexcept>
#include <algorithm>
#include <string>

void ICGenerator::visitCompound(CompoundNode* n) {
    if (!n) return;
    for (ASTNode* stmt : n->statements) {
        visitNode(stmt);
    }
}

void ICGenerator::visitIf(IfNode* n) {
    if (!n) return;
    visitNode(n->condition);
    int jpcLine = currentLine();
    emit("JPC", 0, 0);

    visitNode(n->thenBranch);

    if (n->elseBranch) {
        int jmpLine = currentLine();
        emit("JMP", 0, 0);
        backpatch(jpcLine, currentLine());
        visitNode(n->elseBranch);
        backpatch(jmpLine, currentLine());
    } else {
        backpatch(jpcLine, currentLine());
    }
}

void ICGenerator::visitWhile(WhileNode* n) {
    if (!n) return;
    int startLine = currentLine();
    visitNode(n->condition);
    int jpcLine = currentLine();
    emit("JPC", 0, 0);

    visitNode(n->body);

    emit("JMP", 0, startLine);
    backpatch(jpcLine, currentLine());
}

void ICGenerator::visitFor(ForNode* n) {
    if (!n) return;
    int idx = -1;
    for (int i = symTable_.tabSize() - 1; i >= 1; --i) {
        if (symTable_.getTabEntry(i).name == n->varName) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        throw std::runtime_error("Runtime Error: Loop variable not found: " + n->varName);
    }
    ms3::TabEntry& entry = symTable_.getTabEntry(idx);

    // Evaluate starting value and store into variable
    visitNode(n->fromExpr);
    emit("STO", entry.lev, entry.adr);

    int condStart = currentLine();
    
    // Evaluate current value of loop variable
    emit("LOD", entry.lev, entry.adr);
    // Evaluate boundary expression
    visitNode(n->toExpr);

    // Compare
    if (n->downto) {
        emit("OPR", 0, 10); // GEQ (>=)
    } else {
        emit("OPR", 0, 12); // LEQ (<=)
    }

    int jpcLine = currentLine();
    emit("JPC", 0, 0);

    // Body
    visitNode(n->body);

    // Step (increment / decrement loop variable)
    emit("LOD", entry.lev, entry.adr);
    emit("LIT", 0, 1);
    if (n->downto) {
        emit("OPR", 0, 3); // SUB
    } else {
        emit("OPR", 0, 2); // ADD
    }
    emit("STO", entry.lev, entry.adr);

    emit("JMP", 0, condStart);
    backpatch(jpcLine, currentLine());
}

void ICGenerator::visitRepeat(RepeatNode* n) {
    if (!n) return;
    int startLine = currentLine();
    for (ASTNode* stmt : n->body) {
        visitNode(stmt);
    }
    visitNode(n->condition);
    // Repeat runs until condition is true, i.e., jumps if condition is false
    emit("JPC", 0, startLine);
}

void ICGenerator::visitCase(CaseNode* n) {
    if (!n) return;
    
    std::vector<int> endJmps;
    
    for (CaseBranchNode* branch : n->branches) {
        std::vector<int> labelJpcs;
        for (ASTNode* label : branch->labels) {
            visitNode(n->expr);
            visitNode(label);
            emit("OPR", 0, 7); // EQL
            
            // Negate condition so JPC jumps to statement if true
            emit("LIT", 0, 0);
            emit("OPR", 0, 7); // EQL to 0 (NOT)
            
            labelJpcs.push_back(currentLine());
            emit("JPC", 0, 0);
        }
        
        int skipBranchJmp = currentLine();
        emit("JMP", 0, 0);
        
        int stmtStart = currentLine();
        for (int jpcLine : labelJpcs) {
            backpatch(jpcLine, stmtStart);
        }
        
        visitNode(branch->statement);
        
        endJmps.push_back(currentLine());
        emit("JMP", 0, 0);
        
        backpatch(skipBranchJmp, currentLine());
    }
    
    int endLine = currentLine();
    for (int jmpLine : endJmps) {
        backpatch(jmpLine, endLine);
    }
}

void ICGenerator::visitProcCall(ProcCallNode* n) {
    if (!n) return;
    std::string lowerName = n->name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });

    if (lowerName == "writeln" || lowerName == "write") {
        if (lowerName == "write") {
            for (ASTNode* arg : n->args) {
                visitNode(arg);
                emit("OPR", 0, 13); // WRT: print top of stack without newline
            }
        } else {
            if (n->args.empty()) {
                throw std::runtime_error("Runtime Error: Writeln requires at least one argument");
            }

            for (size_t i = 0; i < n->args.size(); i++) {
                visitNode(n->args[i]);
                emit("OPR", 0, i + 1 == n->args.size() ? 14 : 13);
            }
        }
    } else {
        for (ASTNode* arg : n->args) {
            visitNode(arg);
        }
        auto it = funcLabels_.find(n->name);
        if (it != funcLabels_.end()) {
            emit("CAL", 0, it->second);
        }
    }
}
