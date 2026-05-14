#pragma once
#include <string>
#include <vector>

#define TYPE_NONE 0
#define TYPE_INTEGER 1
#define TYPE_REAL 2
#define TYPE_BOOLEAN 3
#define TYPE_CHAR 4
#define TYPE_STRING 5
#define TYPE_ARRAY 6
#define TYPE_RECORD 7
#define TYPE_VOID 8
#define OBJ_VARIABLE 0
#define OBJ_CONSTANT 1
#define OBJ_TYPE 2
#define OBJ_PROCEDURE 3
#define OBJ_FUNCTION 4
#define OBJ_PROGRAM 5

namespace ms3 {

struct TabEntry {
  std::string name;
  int obj = OBJ_VARIABLE; // object kind
  int type = TYPE_NONE; // type code
  int ref = 0; // reference to atab/btab
  int lev = 0; // lexical level
  int adr = 0; // address
  int link = -1; // link for record field chain (-1 = end)
};

struct AtabEntry {
  int xtyp = TYPE_INTEGER; // index type
  int etyp = TYPE_NONE; // element type
  int eref = 0; // element ref (for nested array/record)
  int low = 0;
  int high = 0;
};

struct BtabEntry {
  int last = -1; // index of last field in tab
  int lpar = 0;  // number of parameters (for procedures/functions)
  int psize = 0;
  int vsize = 0;
};

class SymbolTable {
public:
  void initialize();
  int lookup(const std::string &name); // Lookup identifier from innermost scope outward.
  TabEntry &getTabEntry(int index);
  AtabEntry &getAtabEntry(int index);
  BtabEntry &getBtabEntry(int index);

private:
  std::vector<TabEntry> tab_;
  std::vector<AtabEntry> atab_;
  std::vector<BtabEntry> btab_;
};

} // namespace ms3
