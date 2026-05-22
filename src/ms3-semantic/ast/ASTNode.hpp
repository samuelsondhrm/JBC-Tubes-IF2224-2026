#pragma once
#include <string>
#include <vector>

struct SemanticInfo {
    int type_code = 0; // kode tipe
    int tab_index = -1; // indeks di tab (-1 jika bukan identifier)
    int lev = 0; // lexical level
    int ref = 0; // ref ke atab/btab (untuk array/record)
};

enum class ASTKind {
    Program, Block,
    VarDecl, ConstDecl, TypeDecl, ProcDecl, FuncDecl, Param,
    SimpleType, ArrayType, RecordType, Range, Enumerated, FieldDecl,
    Assign, Compound, If, While, For, Repeat, Case, CaseBranch, ProcCall,
    BinOp, UnaryOp, Var, ArrayAccess, RecordAccess,
    IntLit, RealLit, CharLit, StringLit, BoolLit, FuncCall
};

struct ASTNode {
    ASTKind kind;
    int line = 0; // untuk pesan error
    SemanticInfo sem; // diisi oleh Semantic Visitor
    virtual ~ASTNode() = default;
};


struct ProgramNode : ASTNode {
    std::string name;
    std::vector<ASTNode*> declarations;
    ASTNode* body;
};

struct BlockNode : ASTNode {
    std::vector<ASTNode*> declarations;
    ASTNode* body;
};

struct VarDeclNode : ASTNode {
    std::vector<std::string> names;
    ASTNode* typeNode; 
};

struct ConstDeclNode : ASTNode {
    std::string name;
    ASTNode* value;
};

struct TypeDeclNode : ASTNode {
    std::string name;
    ASTNode* typeNode;
};

struct ParamNode : ASTNode {
    std::vector<std::string> names;
    ASTNode* typeNode;
    bool isByRef = false;
};

struct ProcDeclNode : ASTNode {
    std::string name;
    std::vector<ParamNode*> params;
    BlockNode* block;
};

struct FuncDeclNode : ASTNode {
    std::string name;
    std::vector<ParamNode*> params;
    std::string returnTypeName;
    BlockNode* block;
};

struct SimpleTypeNode : ASTNode {
    std::string typeName;
};

struct RangeNode : ASTNode {
    ASTNode* low;
    ASTNode* high;
};

struct ArrayTypeNode : ASTNode {
    ASTNode* indexType;
    ASTNode* elementType;
};

struct FieldDeclNode : ASTNode {
    std::vector<std::string> names;
    ASTNode* typeNode;
};

struct RecordTypeNode : ASTNode {
    std::vector<FieldDeclNode*> fields;
};

struct EnumeratedNode : ASTNode {
    std::vector<std::string> values;
};

struct CompoundNode : ASTNode {
    std::vector<ASTNode*> statements;
};

struct AssignNode : ASTNode {
    ASTNode* target;
    ASTNode* value;
};

struct IfNode : ASTNode {
    ASTNode* condition;
    ASTNode* thenBranch;
    ASTNode* elseBranch = nullptr;
};

struct WhileNode : ASTNode {
    ASTNode* condition;
    ASTNode* body;
};

struct ForNode : ASTNode {
    std::string varName;
    ASTNode* fromExpr;
    ASTNode* toExpr;
    bool downto = false;
    ASTNode* body;
};

struct RepeatNode : ASTNode {
    std::vector<ASTNode*> body;
    ASTNode* condition;
};

struct CaseBranchNode : ASTNode {
    std::vector<ASTNode*> labels;
    ASTNode* statement;
};

struct CaseNode : ASTNode {
    ASTNode* expr;
    std::vector<CaseBranchNode*> branches;
};

struct ProcCallNode : ASTNode {
    std::string name;
    std::vector<ASTNode*> args;
};

struct BinOpNode : ASTNode {
    std::string op;
    ASTNode* left;
    ASTNode* right;
};

struct UnaryOpNode : ASTNode {
    std::string op;
    ASTNode* operand;
};

struct VarNode : ASTNode {
    std::string name;
};

struct ArrayAccessNode : ASTNode {
    ASTNode* array;
    ASTNode* index;
};

struct RecordAccessNode : ASTNode {
    ASTNode* record;
    std::string field;
};

struct IntLitNode  : ASTNode { 
    int value; 
};
struct RealLitNode : ASTNode { 
    double value; 
};
struct CharLitNode : ASTNode { 
    char   value; 
};
struct StringLitNode : ASTNode { 
    std::string value; 
};
struct BoolLitNode : ASTNode { 
    bool   value; 
};

struct FuncCallNode : ASTNode {
    std::string name;
    std::vector<ASTNode*> args;
};
