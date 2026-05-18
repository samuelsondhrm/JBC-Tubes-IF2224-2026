#pragma once
#include <string>
#include <vector>
#include <iostream>

#define TYPE_NONE 0
#define TYPE_INTEGER 1
#define TYPE_REAL 2
#define TYPE_BOOLEAN 3
#define TYPE_CHAR 4
#define TYPE_STRING 5
#define TYPE_ARRAY 6
#define TYPE_RECORD 7
#define TYPE_VOID 8
#define TYPE_SUBRANGE 9
#define OBJ_VARIABLE 0
#define OBJ_CONSTANT 1
#define OBJ_TYPE 2
#define OBJ_PROCEDURE 3
#define OBJ_FUNCTION 4
#define OBJ_PROGRAM 5

namespace ms3 {

struct TabEntry {
  std::string name;
  int obj = OBJ_VARIABLE;
  int type = TYPE_NONE;
  int ref = 0;
  int lev = 0;
  int adr = 0;
  int link = -1;
};

struct AtabEntry {
  int xtyp = TYPE_INTEGER;
  int etyp = TYPE_NONE;
  int eref = 0;
  int low = 0;
  int high = 0;
};

struct BtabEntry {
  int last = -1;
  int lpar = 0;
  int psize = 0;
  int vsize = 0;
};

class SymbolTable {
public:
  SymbolTable() = default;

  void initialize();

  int  openBlock();
  void closeBlock(int btabIdx, int& lastIdx);

  int enter(const std::string& name, int obj, int typeCode, int ref = 0, int nrm = 1, int lev = 0, int adr = 0);

  int lookup(const std::string& name) const;
  int lookupCurrentScope(const std::string& name) const;

  TabEntry&  getTabEntry(int idx);
  BtabEntry& getBtabEntry(int idx);
  AtabEntry& getAtabEntry(int idx);

  int enterArray(int xtyp, int etyp, int eref, int low, int high, int elsz);

  int typeSize(int typeCode, int ref = 0) const;

  void printTab(std::ostream& out) const;
  void printBtab(std::ostream& out) const;
  void printAtab(std::ostream& out) const;

  int currentLevel() const;
  int tabSize() const;
  int btabSize() const;
  int atabSize() const;

private:
  std::vector<TabEntry>  tab_;
  std::vector<AtabEntry>  atab_;
  std::vector<BtabEntry> btab_;

  int currentScopeHead_ = -1;

  std::vector<int> scopeStack_;
  std::vector<int> savedScopeHeads_;
};

} // namespace ms3
