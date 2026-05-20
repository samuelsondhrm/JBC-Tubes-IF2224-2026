#include "./SymbolTable.hpp"

namespace ms3 {

void SymbolTable::initialize() {
    tab_.clear();
    atab_.clear();
    btab_.clear();
    scopeStack_.clear();
    savedScopeHeads_.clear();
    currentScopeHead_ = -1;

    tab_.push_back({"", OBJ_VARIABLE, TYPE_NONE, 0, 1, 0, 0, -1});
    openBlock(); 
    enter("Real", OBJ_TYPE, TYPE_REAL, 0, 1, 0, 0);
    enter("Integer", OBJ_TYPE, TYPE_INTEGER, 0, 1, 0, 0);
    enter("Char", OBJ_TYPE, TYPE_CHAR, 0, 1, 0, 0);
    enter("Boolean", OBJ_TYPE, TYPE_BOOLEAN, 0, 1, 0, 0);
    enter("String", OBJ_TYPE, TYPE_STRING, 0, 1, 0, 0);
    enter("True", OBJ_CONSTANT, TYPE_BOOLEAN, 0, 1, 0, 1);
    enter("False", OBJ_CONSTANT, TYPE_BOOLEAN, 0, 1, 0, 0);
}

int SymbolTable::openBlock() {
    int btabIdx = static_cast<int>(btab_.size());
    btab_.push_back({-1, 0, 0, 0});
    scopeStack_.push_back(btabIdx);
    savedScopeHeads_.push_back(currentScopeHead_);
    currentScopeHead_ = -1;
    return btabIdx;
}

void SymbolTable::closeBlock(int btabIdx, int& lastIdx) {
    btab_[btabIdx].last = currentScopeHead_;
    lastIdx = currentScopeHead_;

    if (!savedScopeHeads_.empty()) {
        currentScopeHead_ = savedScopeHeads_.back();
        savedScopeHeads_.pop_back();
    } else {
        currentScopeHead_ = -1;
    }

    if (!scopeStack_.empty()) {
        scopeStack_.pop_back();
    }
}

int SymbolTable::enter(const std::string& name, int obj, int typeCode,
                       int ref, int nrm, int lev, int adr) {
    int idx = static_cast<int>(tab_.size());
    TabEntry entry;
    entry.name = name;
    entry.obj = obj;
    entry.type = typeCode;
    entry.ref = ref;
    entry.nrm = nrm;
    entry.lev = lev;
    entry.adr = adr;
    entry.link = currentScopeHead_;

    tab_.push_back(entry);
    currentScopeHead_ = idx;

    if (!scopeStack_.empty()) {
        btab_[scopeStack_.back()].last = idx;
    }

    return idx;
}

int SymbolTable::lookup(const std::string& name) const {
    for (int i = static_cast<int>(scopeStack_.size()) - 1; i >= 0; i--) {
        int btabIdx = scopeStack_[i];
        int idx = btab_[btabIdx].last;
        while (idx >= 1) {
            if (tab_[idx].name == name) {
                return idx;
            }
            idx = tab_[idx].link;
        }
    }
    return -1;
}

int SymbolTable::lookupCurrentScope(const std::string& name) const {
    int idx = currentScopeHead_;
    while (idx >= 1) {
        if (tab_[idx].name == name) {
            return idx;
        }
        idx = tab_[idx].link;
    }
    return -1;
}

TabEntry& SymbolTable::getTabEntry(int idx) {
    return tab_[idx];
}

BtabEntry& SymbolTable::getBtabEntry(int idx) {
    return btab_[idx];
}

AtabEntry& SymbolTable::getAtabEntry(int idx) {
    return atab_[idx];
}

int SymbolTable::enterArray(int xtyp, int etyp, int eref, int low, int high, int elsz) {
    int idx = static_cast<int>(atab_.size());
    AtabEntry entry;
    entry.xtyp = xtyp;
    entry.etyp = etyp;
    entry.eref = eref;
    entry.low = low;
    entry.high = high;
    entry.elsz = elsz; 
    entry.size = (high - low + 1) * elsz; 
    atab_.push_back(entry);
    return idx;
}

int SymbolTable::typeSize(int typeCode, int ref) const {
    switch (typeCode) {
        case TYPE_INTEGER:
        case TYPE_BOOLEAN:
        case TYPE_CHAR:
        case TYPE_SUBRANGE:
            return 1;
        case TYPE_REAL:
            return 1;
        case TYPE_STRING:
            return 1;
        case TYPE_ARRAY: {
            if (ref >= 0 && ref < static_cast<int>(atab_.size())) {
                return atab_[ref].size > 0 ? atab_[ref].size : 1;
            }
            return 1;
        }
        case TYPE_RECORD: {
            if (ref >= 0 && ref < static_cast<int>(btab_.size())) {
                return btab_[ref].vsze > 0 ? btab_[ref].vsze : 1;
            }
            return 1;
        }
        case TYPE_VOID:
            return 0;
        default:
            return 1;
    }
}

void SymbolTable::printTab(std::ostream& out) const {
    out << "\n=== TAB (Symbol Table) ===\n";
    out << std::setw(4) << "idx" << " "
        << std::setw(15) << std::left << "name" << " "
        << std::setw(4) << std::right << "obj" << " "
        << std::setw(4) << "typ" << " "
        << std::setw(4) << "ref" << " "
        << std::setw(4) << "nrm" << " "
        << std::setw(4) << "lev" << " "
        << std::setw(4) << "adr" << " "
        << std::setw(4) << "link" << "\n";
    for (int i = 1; i < static_cast<int>(tab_.size()); i++) {
        const auto& e = tab_[i];
        out << std::setw(4) << i << " "
            << std::setw(15) << std::left << e.name << " "
            << std::setw(4) << std::right << e.obj << " "
            << std::setw(4) << e.type << " "
            << std::setw(4) << e.ref << " " 
            << std::setw(4) << e.nrm << " "
            << std::setw(4) << e.lev << " "
            << std::setw(4) << e.adr << " "
            << std::setw(4) << e.link << "\n";
    }
}

void SymbolTable::printBtab(std::ostream& out) const {
    out << "\n=== BTAB (Block Table) ===\n";
    out << std::setw(4) << "idx" << " "
        << std::setw(6) << "last" << " "
        << std::setw(6) << "lpar" << " "
        << std::setw(6) << "psze" << " "
        << std::setw(6) << "vsze" << "\n";
    for (int i = 0; i < static_cast<int>(btab_.size()); i++) {
        const auto& e = btab_[i];
        out << std::setw(4) << i << " "
            << std::setw(6) << e.last << " "
            << std::setw(6) << e.lpar << " "
            << std::setw(6) << e.psze << " "
            << std::setw(6) << e.vsze << "\n";
    }
}

void SymbolTable::printAtab(std::ostream& out) const {
    out << "\n=== ATAB (Array Table) ===\n";
    out << std::setw(4) << "idx" << " "
        << std::setw(6) << "xtyp" << " "
        << std::setw(6) << "etyp" << " "
        << std::setw(6) << "eref" << " "
        << std::setw(6) << "low" << " "
        << std::setw(6) << "high" << " "
        << std::setw(6) << "elsz" << " " 
        << std::setw(6) << "size" << "\n";
    for (int i = 0; i < static_cast<int>(atab_.size()); i++) {
        const auto& e = atab_[i];
        out << std::setw(4) << i << " "
            << std::setw(6) << e.xtyp << " "
            << std::setw(6) << e.etyp << " "
            << std::setw(6) << e.eref << " "
            << std::setw(6) << e.low << " "
            << std::setw(6) << e.high << " " 
            << std::setw(6) << e.elsz << " " 
            << std::setw(6) << e.size << "\n"; 
    }
}

int SymbolTable::currentLevel() const {
    return static_cast<int>(scopeStack_.size()) - 1;
}

int SymbolTable::tabSize() const {
    return static_cast<int>(tab_.size());
}

int SymbolTable::btabSize() const {
    return static_cast<int>(btab_.size());
}

int SymbolTable::atabSize() const {
    return static_cast<int>(atab_.size());
}

} // namespace ms3
