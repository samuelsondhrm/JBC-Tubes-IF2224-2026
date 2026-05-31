#include "ICGenerator.hpp"
#include <fstream>
#include <stdexcept>

ICGenerator::ICGenerator(ms3::SymbolTable& symTable) : symTable_(symTable) {}

// Tambahkan satu instruksi IC ke daftar dan naikkan nomor baris berikutnya
void ICGenerator::emit(const std::string& op, int level, int arg) {
    code_.push_back(ICInstruction(nextLine_++, op, level, arg));
}

// Kembalikan nomor baris yang akan dipakai oleh emit berikutnya
int ICGenerator::currentLine() const { return nextLine_; }

// Isi arg instruksi di baris targetLine dengan value (digunakan untuk menambal JMP/JPC)
void ICGenerator::backpatch(int targetLine, int value) {
    code_[targetLine].arg = value;
}

// Teruskan node ke visitor yang sesuai berdasarkan jenis node-nya
void ICGenerator::visitNode(ASTNode* node) {
    if (!node) return;
    switch (node->kind) {
        case ASTKind::Program:      visitProgram(static_cast<ProgramNode*>(node));         break;
        case ASTKind::Block:        visitBlock(static_cast<BlockNode*>(node));              break;
        case ASTKind::Assign:       visitAssign(static_cast<AssignNode*>(node));            break;
        case ASTKind::BinOp:        visitBinOp(static_cast<BinOpNode*>(node));              break;
        case ASTKind::UnaryOp:      visitUnaryOp(static_cast<UnaryOpNode*>(node));          break;
        case ASTKind::Var:          visitVar(static_cast<VarNode*>(node));                  break;
        case ASTKind::ArrayAccess:  visitArrayAccess(static_cast<ArrayAccessNode*>(node));  break;
        case ASTKind::RecordAccess: visitRecordAccess(static_cast<RecordAccessNode*>(node));break;
        case ASTKind::IntLit:       visitIntLit(static_cast<IntLitNode*>(node));            break;
        case ASTKind::RealLit:      visitRealLit(static_cast<RealLitNode*>(node));          break;
        case ASTKind::CharLit:      visitCharLit(static_cast<CharLitNode*>(node));          break;
        case ASTKind::StringLit:    visitStringLit(static_cast<StringLitNode*>(node));      break;
        case ASTKind::BoolLit:      visitBoolLit(static_cast<BoolLitNode*>(node));          break;
        case ASTKind::FuncCall:     visitFuncCall(static_cast<FuncCallNode*>(node));        break;
        case ASTKind::Compound:     visitCompound(static_cast<CompoundNode*>(node));        break;
        case ASTKind::If:           visitIf(static_cast<IfNode*>(node));                    break;
        case ASTKind::While:        visitWhile(static_cast<WhileNode*>(node));              break;
        case ASTKind::For:          visitFor(static_cast<ForNode*>(node));                  break;
        case ASTKind::Repeat:       visitRepeat(static_cast<RepeatNode*>(node));            break;
        case ASTKind::Case:         visitCase(static_cast<CaseNode*>(node));                break;
        case ASTKind::ProcCall:     visitProcCall(static_cast<ProcCallNode*>(node));        break;
        case ASTKind::ProcDecl:     visitProcDecl(static_cast<ProcDeclNode*>(node));        break;
        case ASTKind::FuncDecl:     visitFuncDecl(static_cast<FuncDeclNode*>(node));        break;
        // VarDecl, ConstDecl, TypeDecl tidak perlu emit IC karna sudah ditangani M3
        default: break;
    }
}

// Emit frame program utama, lewati subprogram dengan JMP, lalu jalankan body
void ICGenerator::visitProgram(ProgramNode* n) {
    int varCount = 0;
    for (ASTNode* decl : n->declarations) {
        if (decl->kind == ASTKind::VarDecl) {
            varCount += static_cast<VarDeclNode*>(decl)->names.size();
        }
    }
    emit("INT", 0, 3 + varCount);

    bool hasSubprogram = false;
    for (ASTNode* decl : n->declarations) {
        if (decl->kind == ASTKind::ProcDecl || decl->kind == ASTKind::FuncDecl) {
            hasSubprogram = true;
            break;
        }
    }

    int jmpLine = -1;
    if (hasSubprogram) {
        jmpLine = currentLine();
        emit("JMP", 0, 0);
    }

    for (ASTNode* decl : n->declarations) {
        if (decl->kind == ASTKind::ProcDecl || decl->kind == ASTKind::FuncDecl) {
            visitNode(decl);
        }
    }

    if (hasSubprogram) {
        backpatch(jmpLine, currentLine());
    }

    visitNode(n->body);
    emit("RET", 0, 0);
}

// Emit deklarasi non-variabel dan body untuk blok subprogram
void ICGenerator::visitBlock(BlockNode* n) {
    for (ASTNode* decl : n->declarations) {
        if (decl->kind != ASTKind::VarDecl
            && decl->kind != ASTKind::ConstDecl
            && decl->kind != ASTKind::TypeDecl) {
            visitNode(decl);
        }
    }
    visitNode(n->body);
}

// Entry point: reset state lalu mulai traversal dari root AST
std::vector<ICInstruction> ICGenerator::generate(ASTNode* root) {
    code_.clear();
    nextLine_ = 0;
    visitNode(root);
    return code_;
}

// Tulis daftar instruksi IC ke file teks dengan format: baris op level arg
void ICGenerator::writeToFile(const std::vector<ICInstruction>& code,
                               const std::string& filename) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Cannot open output file: " + filename);
    for (const auto& instr : code) {
        out << instr.line << " " << instr.op << " "
            << instr.level << " " << instr.arg << "\n";
    }
}
