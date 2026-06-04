#include "SemanticAnalyzer.hpp"
#include "../ast/ASTNode.hpp"
#include "../symtable/SymbolTable.hpp"
#include "../type/TypeChecker.hpp"
#include <algorithm>
#include <string>
#include <functional>

namespace {

    int resolveTypeNameHelper(ms3::SymbolTable& symTable, const std::string& name, int line, const std::function<void(const std::string&, int)>& errCb) {
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (lower == "integer") return TYPE_INTEGER;
        if (lower == "real")    return TYPE_REAL;
        if (lower == "char")    return TYPE_CHAR;
        if (lower == "boolean") return TYPE_BOOLEAN;
        if (lower == "string")  return TYPE_STRING;

        int idx = symTable.lookup(name);
        if (idx == -1) {
            errCb("Undefined type: " + name, line);
            return TYPE_NONE;
        }
        
        ms3::TabEntry& e = symTable.getTabEntry(idx);
        if (e.obj != OBJ_TYPE) {
            errCb("'" + name + "' is not a type", line);
            return TYPE_NONE;
        }
        
        return e.type;
    }

    int offsetTracker[100] = {0};
}

void SemanticAnalyzer::visitProgram(ProgramNode* node) {
    int btabIdx = symTable.openBlock(); 
    currentLevel_ = 0;
    offsetTracker[currentLevel_] = 0;
    
    int idx = symTable.enter(node->name, OBJ_PROGRAM, TYPE_VOID, 0, 1, 0, 0);

    node->sem.tab_index = idx;
    node->sem.type_code = TYPE_VOID;
    node->sem.lev = currentLevel_;

    for (ASTNode* decl : node->declarations) {
        visitNode(decl);
    }

    if (node->body) {
        visitNode(node->body); 
    }

    int lastIdx = -1;
    symTable.closeBlock(btabIdx, lastIdx);
}

void SemanticAnalyzer::visitBlock(BlockNode* node) {
    for (ASTNode* decl : node->declarations) {
        visitNode(decl);
    }
    if (node->body) {
        visitNode(node->body);
    }
}

void SemanticAnalyzer::visitVarDecl(VarDeclNode* node) {
    auto errCb = [&](const std::string& msg, int line) { this->semanticError(msg, line); };
    
    int type_code = TYPE_NONE;
    int ref = 0;

    // Evaluasi jenis dan kode tipe data variabel
    if (node->typeNode) {
        if (node->typeNode->kind == ASTKind::SimpleType) {
            auto* st = static_cast<SimpleTypeNode*>(node->typeNode);
            type_code = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
        } 
        else if (node->typeNode->kind == ASTKind::ArrayType) {
            auto* arrType = static_cast<ArrayTypeNode*>(node->typeNode);
            type_code = TYPE_ARRAY;
            
            int low = 0, high = 0;
            int xtyp = TYPE_INTEGER; // Default index type
            int etyp = TYPE_NONE;    
            int eref = 0;            
            
            // Evaluasi kondisi jika Index Type berupa Range [low..high]
            if (arrType->indexType && arrType->indexType->kind == ASTKind::Range) {
                auto* rangeNode = static_cast<RangeNode*>(arrType->indexType);
                if (rangeNode->low && rangeNode->low->kind == ASTKind::IntLit)
                    low = static_cast<IntLitNode*>(rangeNode->low)->value;
                if (rangeNode->high && rangeNode->high->kind == ASTKind::IntLit)
                    high = static_cast<IntLitNode*>(rangeNode->high)->value;
                
                if (low > high) {
                    semanticError("Subrange lower bound exceeds upper bound", rangeNode->line);
                }
            }
            // Evaluasi kondisi jika Index Type berupa Named Type langsung
            else if (arrType->indexType && arrType->indexType->kind == ASTKind::SimpleType) {
                auto* st = static_cast<SimpleTypeNode*>(arrType->indexType);
                xtyp = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
                
                if (xtyp == TYPE_REAL) {
                    semanticError("Array index type cannot be Real", node->line);
                }
            }
            
            // Ekstrak tipe elemen array
            if (arrType->elementType && arrType->elementType->kind == ASTKind::SimpleType) {
                auto* st = static_cast<SimpleTypeNode*>(arrType->elementType);
                etyp = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
            }
            
            int elsz = symTable.typeSize(etyp, eref);
            ref = symTable.enterArray(xtyp, etyp, eref, low, high, elsz);
        } 
        else if (node->typeNode->kind == ASTKind::RecordType) {
            auto* recType = static_cast<RecordTypeNode*>(node->typeNode);
            type_code = TYPE_RECORD;
            
            int btabIdx = symTable.openBlock();
            ref = btabIdx; 
            
            currentLevel_++;
            int fieldAdr = 0;
            int lastFieldIdx = -1;
            
            // Daftarkan seluruh komponen field internal Record
            for (FieldDeclNode* field : recType->fields) {
                int fieldType = TYPE_NONE;
                if (field->typeNode && field->typeNode->kind == ASTKind::SimpleType) {
                    auto* st = static_cast<SimpleTypeNode*>(field->typeNode);
                    fieldType = resolveTypeNameHelper(symTable, st->typeName, field->line, errCb);
                }
                
                for (const std::string& fName : field->names) {
                    if (symTable.lookupCurrentScope(fName) != -1) {
                        semanticError("Identifier '" + fName + "' already declared in this scope", field->line);
                        continue;
                    }
                    lastFieldIdx = symTable.enter(fName, OBJ_VARIABLE, fieldType, 0, 1, currentLevel_, fieldAdr);
                    fieldAdr += symTable.typeSize(fieldType, 0);
                }
            }
            
            // Tutup blok dan pasang indeks field terakhir untuk struktur data linked list btab
            symTable.closeBlock(btabIdx, lastFieldIdx);
            currentLevel_--;
        }
    }

    int& adr = offsetTracker[currentLevel_]; 
    
    // Daftarkan masing-masing nama variabel utama ke scope saat ini
    for (const std::string& name : node->names) {
        if (symTable.lookupCurrentScope(name) != -1) {
            semanticError("Identifier '" + name + "' already declared in this scope", node->line);
            continue;
        }

        int idx = symTable.enter(name, OBJ_VARIABLE, type_code, ref, 1, currentLevel_, adr);
        
        // Lengkapi seluruh komponen dekorasi SemanticInfo AST
        node->sem.tab_index = idx;
        node->sem.type_code = type_code;
        node->sem.lev = currentLevel_;
        node->sem.ref = ref; 
        
        adr += symTable.typeSize(type_code, ref);
    }
}

void SemanticAnalyzer::visitConstDecl(ConstDeclNode* node) {
    int type_code = TYPE_NONE;
    int val = 0;

    // Evaluasi tipe dan nilai primitif dari literal
    if (node->value) {
        visitExpr(node->value); 
        type_code = node->value->sem.type_code;
        
        if (node->value->kind == ASTKind::IntLit) {
            val = static_cast<IntLitNode*>(node->value)->value;
        } else if (node->value->kind == ASTKind::CharLit) {
            val = static_cast<CharLitNode*>(node->value)->value;
        } else if (node->value->kind == ASTKind::BoolLit) {
            val = static_cast<BoolLitNode*>(node->value)->value ? 1 : 0;
        }
        // Catatan: RealLit, StringLit
    }

    if (symTable.lookupCurrentScope(node->name) != -1) {
        semanticError("Identifier '" + node->name + "' already declared in this scope", node->line);
        return;
    }

    int idx = symTable.enter(node->name, OBJ_CONSTANT, type_code, 0, 1, currentLevel_, val);
    
    // Anotasi node
    node->sem.tab_index = idx;
    node->sem.type_code = type_code;
    node->sem.lev = currentLevel_;
}

void SemanticAnalyzer::visitTypeDecl(TypeDeclNode* node) {
    auto errCb = [&](const std::string& msg, int line) { this->semanticError(msg, line); };
    
    int type_code = TYPE_NONE;
    int ref = 0;

    // 1. Resolve base type
    if (node->typeNode) {
        if (node->typeNode->kind == ASTKind::SimpleType) {
            auto* st = static_cast<SimpleTypeNode*>(node->typeNode);
            type_code = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
        } 
        else if (node->typeNode->kind == ASTKind::ArrayType) {
            auto* arrType = static_cast<ArrayTypeNode*>(node->typeNode);
            type_code = TYPE_ARRAY;
            
            int low = 0, high = 0;
            int xtyp = TYPE_INTEGER; 
            int etyp = TYPE_NONE;    
            int eref = 0;            
            
            if (arrType->indexType && arrType->indexType->kind == ASTKind::Range) {
                auto* rangeNode = static_cast<RangeNode*>(arrType->indexType);
                if (rangeNode->low && rangeNode->low->kind == ASTKind::IntLit)
                    low = static_cast<IntLitNode*>(rangeNode->low)->value;
                if (rangeNode->high && rangeNode->high->kind == ASTKind::IntLit)
                    high = static_cast<IntLitNode*>(rangeNode->high)->value;
                
                if (low > high) {
                    semanticError("Subrange lower bound exceeds upper bound", rangeNode->line);
                }
            } else if (arrType->indexType && arrType->indexType->kind == ASTKind::SimpleType) {
                auto* st = static_cast<SimpleTypeNode*>(arrType->indexType);
                xtyp = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
                if (xtyp == TYPE_REAL) {
                    semanticError("Array index type cannot be Real", node->line);
                }
            }
            
            if (arrType->elementType && arrType->elementType->kind == ASTKind::SimpleType) {
                auto* st = static_cast<SimpleTypeNode*>(arrType->elementType);
                etyp = resolveTypeNameHelper(symTable, st->typeName, node->line, errCb);
            }
            
            int elsz = symTable.typeSize(etyp, eref);
            ref = symTable.enterArray(xtyp, etyp, eref, low, high, elsz);
        } 
        else if (node->typeNode->kind == ASTKind::RecordType) {
            auto* recType = static_cast<RecordTypeNode*>(node->typeNode);
            type_code = TYPE_RECORD;
            
            int btabIdx = symTable.openBlock();
            ref = btabIdx; 
            
            currentLevel_++;
            int fieldAdr = 0;
            int lastFieldIdx = -1;
            
            for (FieldDeclNode* field : recType->fields) {
                int fieldType = TYPE_NONE;
                if (field->typeNode && field->typeNode->kind == ASTKind::SimpleType) {
                    auto* st = static_cast<SimpleTypeNode*>(field->typeNode);
                    fieldType = resolveTypeNameHelper(symTable, st->typeName, field->line, errCb);
                }
                
                for (const std::string& fName : field->names) {
                    if (symTable.lookupCurrentScope(fName) != -1) {
                        semanticError("Identifier '" + fName + "' already declared in this scope", field->line);
                        continue;
                    }
                    lastFieldIdx = symTable.enter(fName, OBJ_VARIABLE, fieldType, 0, 1, currentLevel_, fieldAdr);
                    fieldAdr += symTable.typeSize(fieldType, 0);
                }
            }
            
            symTable.closeBlock(btabIdx, lastFieldIdx);
            currentLevel_--;
        }
    }

    // 2. Cek Redeclaration
    if (symTable.lookupCurrentScope(node->name) != -1) {
        semanticError("Identifier '" + node->name + "' already declared in this scope", node->line);
        return;
    }

    // 3. Masukkan ke tabel simbol sebagai OBJ_TYPE
    int idx = symTable.enter(node->name, OBJ_TYPE, type_code, ref, 1, currentLevel_, 0);
    
    // 4. Anotasi AST Node
    node->sem.tab_index = idx;
    node->sem.type_code = type_code;
    node->sem.lev = currentLevel_;
    node->sem.ref = ref;
}

void SemanticAnalyzer::visitProcDecl(ProcDeclNode* node) {
    auto errCb = [&](const std::string& msg, int line) { this->semanticError(msg, line); };

    // 1. Cek redeklarasi nama prosedur
    if (symTable.lookupCurrentScope(node->name) != -1) {
        semanticError("Identifier '" + node->name + "' already declared in this scope", node->line);
    }

    // 2. Register nama prosedur di scope parent sebelum membuka blok baru
    int procIdx = symTable.enter(node->name, OBJ_PROCEDURE, TYPE_VOID, 0, 1, currentLevel_, 0);
    
    node->sem.tab_index = procIdx;
    node->sem.type_code = TYPE_VOID;
    node->sem.lev = currentLevel_;
    
    // 3. Buka scope untuk body prosedur
    int btabIdx = symTable.openBlock();
    currentLevel_++;
    offsetTracker[currentLevel_] = 0;
    int psze = 0;
    int lastParamIdx = -1;

    // 4. Proses parameter prosedur
    for (ParamNode* param : node->params) {
        int param_type_code = TYPE_NONE;
        
        if (param->typeNode && param->typeNode->kind == ASTKind::SimpleType) {
            auto* st = static_cast<SimpleTypeNode*>(param->typeNode);
            param_type_code = resolveTypeNameHelper(symTable, st->typeName, param->line, errCb);
        }

        int nrm = param->isByRef ? 0 : 1;
        for (const std::string& name : param->names) {
            if (symTable.lookupCurrentScope(name) != -1) {
                semanticError("Identifier '" + name + "' already declared in this scope", param->line);
                continue;
            }
            lastParamIdx = symTable.enter(name, OBJ_VARIABLE, param_type_code, 0, nrm, currentLevel_, psze);
            psze += symTable.typeSize(param_type_code, 0); 
        }
    }

    // 5. Simpan metadata parameter ke Btab
    offsetTracker[currentLevel_] = psze;
    ms3::BtabEntry& bEntry = symTable.getBtabEntry(btabIdx);
    bEntry.psize = psze;
    bEntry.lpar = lastParamIdx;

    // 6. Eksekusi blok prosedur
    if (node->block) {
        visitBlock(node->block);
    }

    // 7. Cleanup scope
    int dummyLastIdx = -1;
    symTable.closeBlock(btabIdx, dummyLastIdx);
    currentLevel_--;

    // 8. Hubungkan entri prosedur di tabel parent dengan blok baru
    symTable.getTabEntry(procIdx).ref = btabIdx;
    node->sem.ref = btabIdx; // Anotasi ref untuk Decorated AST Printer
}

void SemanticAnalyzer::visitFuncDecl(FuncDeclNode* node) {
    auto errCb = [&](const std::string& msg, int line) { this->semanticError(msg, line); };

    // 1. Cek redeklarasi nama fungsi di scope saat ini
    if (symTable.lookupCurrentScope(node->name) != -1) {
        semanticError("Identifier '" + node->name + "' already declared in this scope", node->line);
    }

    // 2. Evaluasi Return Type
    int retTypeCode = resolveTypeNameHelper(symTable, node->returnTypeName, node->line, errCb);

    // 3. Register nama fungsi di scope parent
    int funcIdx = symTable.enter(node->name, OBJ_FUNCTION, retTypeCode, 0, 1, currentLevel_, 0);

    node->sem.tab_index = funcIdx;
    node->sem.type_code = retTypeCode;
    node->sem.lev = currentLevel_;

    // 4. Buka scope untuk body fungsi
    int btabIdx = symTable.openBlock();
    currentLevel_++;
    offsetTracker[currentLevel_] = 0;
    int psze = 0;
    int lastParamIdx = -1;

    // 5. Proses parameter fungsi
    for (ParamNode* param : node->params) {
        int param_type_code = TYPE_NONE;
        if (param->typeNode && param->typeNode->kind == ASTKind::SimpleType) {
            auto* st = static_cast<SimpleTypeNode*>(param->typeNode);
            param_type_code = resolveTypeNameHelper(symTable, st->typeName, param->line, errCb);
        }

        int nrm = param->isByRef ? 0 : 1;
        for (const std::string& name : param->names) {
            if (symTable.lookupCurrentScope(name) != -1) {
                semanticError("Identifier '" + name + "' already declared in this scope", param->line);
                continue;
            }
            lastParamIdx = symTable.enter(name, OBJ_VARIABLE, param_type_code, 0, nrm, currentLevel_, psze);
            psze += symTable.typeSize(param_type_code, 0);
        }
    }

    // 6. Simpan metadata parameter ke Btab
    offsetTracker[currentLevel_] = psze;
    ms3::BtabEntry& bEntry = symTable.getBtabEntry(btabIdx);
    bEntry.lpar = lastParamIdx;
    bEntry.psize = psze;

    // 7. Eksekusi blok fungsi
    if (node->block) {
        visitBlock(node->block);
    }

    // 8. Cleanup scope
    int dummyLastIdx = -1;
    symTable.closeBlock(btabIdx, dummyLastIdx);
    currentLevel_--;

    // 9. Hubungkan entri fungsi di tabel parent dengan blok baru
    symTable.getTabEntry(funcIdx).ref = btabIdx;
    node->sem.ref = btabIdx; 
}