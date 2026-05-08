#pragma once
#include <string>
#include <vector>

struct SemanticInfo {
    int type_code  = 0;   // kode tipe
    int tab_index  = -1;  // indeks di tab (-1 jika bukan identifier)
    int lev        = 0;   // lexical level
    int ref        = 0;   // ref ke atab/btab (untuk array/record)
};

enum class ASTKind {
    // Program
    Program, Block,
    // Declarations
    VarDecl, ConstDecl, TypeDecl, ProcDecl, FuncDecl, Param,
    // Types
    SimpleType, ArrayType, RecordType, Range, Enumerated, FieldDecl,
    // Statements
    Assign, Compound, If, While, For, Repeat, Case, CaseBranch, ProcCall,
    // Expressions
    BinOp, UnaryOp, Var, ArrayAccess, RecordAccess,
    IntLit, RealLit, CharLit, StringLit, BoolLit, FuncCall
};

struct ASTNode {
    ASTKind kind;
    int     line = 0;        // untuk pesan error
    SemanticInfo sem;        // diisi oleh Semantic Visitor
    virtual ~ASTNode() = default;
};

// ===== PROGRAM LEVEL =====

struct ProgramNode : ASTNode {
    std::string               name;
    std::vector<ASTNode*>     declarations;  // VarDecl, ConstDecl, TypeDecl, ProcDecl, FuncDecl
    ASTNode*                  body;          // CompoundNode
};

struct BlockNode : ASTNode {
    std::vector<ASTNode*>     declarations;
    ASTNode*                  body;
};

// ===== DECLARATIONS =====

struct VarDeclNode : ASTNode {
    std::vector<std::string>  names;         // bisa multiple: a, b, c : integer
    ASTNode*                  typeNode;      // SimpleTypeNode / ArrayTypeNode / RecordTypeNode
};

struct ConstDeclNode : ASTNode {
    std::string               name;
    ASTNode*                  value;         // ekspresi literal
};

struct TypeDeclNode : ASTNode {
    std::string               name;
    ASTNode*                  typeNode;
};

struct ParamNode : ASTNode {
    std::vector<std::string>  names;
    ASTNode*                  typeNode;
    bool                      isByRef = false;  // var parameter
};

struct ProcDeclNode : ASTNode {
    std::string               name;
    std::vector<ParamNode*>   params;
    BlockNode*                block;
};

struct FuncDeclNode : ASTNode {
    std::string               name;
    std::vector<ParamNode*>   params;
    std::string               returnTypeName;
    BlockNode*                block;
};

// ===== TYPE NODES =====

struct SimpleTypeNode : ASTNode {
    std::string               typeName;     // "integer", "real", "char", "boolean", "string"
};

struct RangeNode : ASTNode {
    ASTNode*                  low;
    ASTNode*                  high;
};

struct ArrayTypeNode : ASTNode {
    ASTNode*                  indexType;    // RangeNode atau SimpleTypeNode
    ASTNode*                  elementType;
};

struct FieldDeclNode : ASTNode {
    std::vector<std::string>  names;
    ASTNode*                  typeNode;
};

struct RecordTypeNode : ASTNode {
    std::vector<FieldDeclNode*> fields;
};

struct EnumeratedNode : ASTNode {
    std::vector<std::string>  values;
};

// ===== STATEMENTS =====

struct CompoundNode : ASTNode {
    std::vector<ASTNode*>     statements;
};

struct AssignNode : ASTNode {
    ASTNode*                  target;       // VarNode / ArrayAccessNode / RecordAccessNode
    ASTNode*                  value;        // ekspresi
};

struct IfNode : ASTNode {
    ASTNode*                  condition;
    ASTNode*                  thenBranch;
    ASTNode*                  elseBranch = nullptr;
};

struct WhileNode : ASTNode {
    ASTNode*                  condition;
    ASTNode*                  body;
};

struct ForNode : ASTNode {
    std::string               varName;
    ASTNode*                  fromExpr;
    ASTNode*                  toExpr;
    bool                      downto = false;
    ASTNode*                  body;
};

struct RepeatNode : ASTNode {
    std::vector<ASTNode*>     body;
    ASTNode*                  condition;
};

struct CaseBranchNode : ASTNode {
    std::vector<ASTNode*>     labels;       // konstanta
    ASTNode*                  statement;
};

struct CaseNode : ASTNode {
    ASTNode*                  expr;
    std::vector<CaseBranchNode*> branches;
};

struct ProcCallNode : ASTNode {
    std::string               name;
    std::vector<ASTNode*>     args;
};

// ===== EXPRESSIONS =====

struct BinOpNode : ASTNode {
    std::string               op;           // "+", "-", "*", "/", "div", "mod", "and", "or", "=", "<>", "<", ">", "<=", ">="
    ASTNode*                  left;
    ASTNode*                  right;
};

struct UnaryOpNode : ASTNode {
    std::string               op;           // "-", "+", "not"
    ASTNode*                  operand;
};

struct VarNode : ASTNode {
    std::string               name;
};

struct ArrayAccessNode : ASTNode {
    ASTNode*                  array;        // VarNode / RecordAccessNode / ArrayAccessNode
    ASTNode*                  index;
};

struct RecordAccessNode : ASTNode {
    ASTNode*                  record;       // VarNode / RecordAccessNode / ArrayAccessNode
    std::string               field;
};

struct IntLitNode  : ASTNode { int    value; };
struct RealLitNode : ASTNode { double value; };
struct CharLitNode : ASTNode { char   value; };
struct StringLitNode : ASTNode { std::string value; };
struct BoolLitNode : ASTNode { bool   value; };

struct FuncCallNode : ASTNode {
    std::string               name;
    std::vector<ASTNode*>     args;
};
