# Arion Compiler - Tugas Besar IF2224 Teori Bahasa Formal dan Otomata

> Milestone 3: Semantic Analyzer

> Milestone 2: Syntax Analyzer

> Milestone 1: Lexical Analyzer

## Daftar Isi
- 

## Deskripsi Tugas Besar

### Milestone 1: Lexical Analyzer

Arion Lexer merupakan milestone pertama dari proyek Compiler Arion — sebuah lexical analyzer (lexer) untuk bahasa pemrograman **Arion**, yang diimplementasikan menggunakan C++17 tanpa menggunakan tools maupun library pembangkit lexer otomatis.

Lexer membaca file source code Arion karakter demi karakter dan menghasilkan urutan **token**. Token adalah unit bermakna terkecil dalam sebuah program (misalnya keyword, identifier, operator, dan literal). Proses ini diimplementasikan secara formal sebagai sebuah **Deterministic Finite Automaton (DFA)**, di mana setiap karakter yang dibaca menyebabkan transisi state hingga mencapai accepting state dan menghasilkan sebuah token.

Program menerima file source code berekstensi `.txt` sebagai input dan menghasilkan daftar token sebagai output, baik ditampilkan pada terminal maupun disimpan ke file output tertentu.

### Milestone 2: Syntax Analyzer

### Milestone 4: Semantic Analyzer

## Struktur Project

```
JBC-Tubes-IF2224-2026
├── CMakeLists.txt
├── Makefile
├── README.md
├── doc/
├── src/
│   ├── ms1-lexer/
│   │   ├── main.cpp
│   │   ├── dfa/
│   │   │   ├── dfa.hpp
│   │   │   └── dfa.cpp
│   │   ├── lexer/
│   │   │   ├── lexer.hpp
│   │   │   ├── lexer.cpp
│   │   │   ├── symbolTable.hpp
│   │   │   └── symbolTable.cpp
│   │   ├── token/
│   │   │   ├── token.hpp
│   │   │   └── token.cpp
│   │   └── util/
│   │       ├── read.hpp
│   │       └── read.cpp
│   ├── ms2-parser
│   │   ├── main.cpp
│   │   ├── ParseNode.cpp
│   │   ├── ParseNode.hpp
│   │   ├── parser_declarations.cpp
│   │   ├── parser_expressions.cpp
│   │   ├── parser_statements.cpp
│   │   ├── parser_subprograms.cpp
│   │   ├── parser_toplevel.cpp
│   │   ├── Parser.cpp
│   │   ├── Parser.hpp
│   │   ├── TreePrinter.cpp
│   │   └── TreePrinter.hpp
│   └── ms3-semantic
│       ├── ast/
│       │   └── ASTNode.hpp
│       ├── builder/
│       │   ├── ASTBuilder.cpp
│       │   └── ASTBuilder.hpp
│       ├── printer/
│       │   ├── DecoratedASTPrinter.cpp
│       │   └── DecoratedASTPrinter.hpp
│       ├── semantic/
│       │   ├── SemanticAnalyzer.cpp
│       │   ├── SemanticAnalyzer.hpp
│       │   ├── visit_declarations.cpp
│       │   ├── visit_expressions.cpp
│       │   └── visit_statements.cpp
│       ├── symtable/
│       │   ├── SymbolTable.cpp
│       │   └── SymbolTable.hpp
│       ├── type/
│       │   ├── TypeChecker.cpp
│       │   └── TypeChecker.hpp
│       └── main.cpp
└── test/
    ├── milestone-1/
    │   ├── test-valid.txt
    │   ├── test-invalid.txt
    │   ├── test-edgecases.txt
    │   ├── test-eql.txt
    │   ├── test-caseinsensitive.txt
    │   ├── test-commentambiguity.txt
    │   ├── test-kwidboundary.txt
    │   ├── test-literaltypes.txt
    │   └── outputs/
    ├── milestone-2
    │   ├── test-01-minimal.txt
    │   ├── test-02-vars.txt
    │   ├── test-03-const.txt
    │   ├── test-04-types.txt
    │   ├── test-05-assign.txt
    │   ├── test-06-ifelse.txt
    │   ├── test-07-loops.txt
    │   ├── test-08-procedure.txt
    │   ├── test-09-function.txt
    │   ├── test-10-expressions.txt
    │   ├── test-11-arrrecord.txt
    │   ├── test-12-case.txt
    │   ├── test-13-proccall.txt
    │   ├── test-14-complex.txt
    │   ├── test-15-alldecl.txt
    │   ├── test-16-comments.txt
    │   ├── test-18-err-wrong-token.txt
    │   ├── test-21-err-bad-for.txt
    │   ├── expected-errors/
    │   └── outputs/
    └── milestone-3/
        ├── test-01-minimal.txt
        ├── test-02-alltypes.txt
        ├── test-03-procfunc.txt
        ├── test-04-controlflow.txt
        ├── test-05-nested-scope.txt
        ├── test-06-assign-compat.txt
        ├── test-07-err-redeclare.txt
        ├── test-08-err-undeclared.txt
        ├── test-09-err-assign-const.txt
        ├── test-10-err-type-mismatch.txt
        ├── test-11-err-nonbool-cond.txt
        ├── test-12-err-nonarray-index.txt
        ├── test-13-err-nonrecord-access.txt
        ├── test-14-err-wrong-args.txt
        ├── test-15-err-for-nonint.txt
        ├── test-16-err-case-type.txt
        ├── run_test.sh
        ├── expected-errors/
        └── outputs/
```

### Architecture

The lexer is split into separated, independently testable components:

| Component     | Path                     | Role                                                                       |
| ------------- | ------------------------ | -------------------------------------------------------------------------- |
| `Token`       | `src/ms1-lexer/token/`   | Data class holding a token's type, value, and line number                  |
| `DFA`         | `src/ms1-lexer/dfa/`     | Pure state machine — reads characters, transitions states, emits tokens    |
| `SymbolTable` | `src/ms1-lexer/lexer/`   | Keyword map used by DFA to distinguish identifiers from reserved words     |
| `Lexer`       | `src/ms1-lexer/lexer/`   | Driver layer — owns a DFA instance, exposes `nextToken()` and `tokenize()` |
| `FileReader`  | `src/ms1-lexer/util/`    | Reads the entire source file into a string                                 |
| `main`        | `src/ms1-lexer/main.cpp` | Entry point — wires all components together and handles output             |

---

## Requirements

| Requirement | Version             | Description                                        |
| ----------- | ------------------- | -------------------------------------------------- |
| `g++`       | ≥ 9.0               | GNU C++ compiler with C++17 support (`-std=c++17`) |
| `make`      | ≥ 3.81              | Build automation tool used to compile the project  |
| OS          | Linux / macOS / WSL | The Makefile uses standard Unix shell commands     |

No third-party libraries are required. The project uses only the C++17 standard library.

---

## Setup

### 1. Clone the repository

```bash
git clone https://github.com/samuelsondhrm/JBC-Tubes-IF2224-2026.git
cd JBC-Tubes-IF2224-2026
```

### 2. Verify compiler is available

```bash
g++ --version
make --version
```

Both commands should return a version without errors. If `g++` is not installed on Ubuntu/Debian:

```bash
sudo apt update && sudo apt install build-essential
```

### 3. Compile

From the **project root** (where `Makefile` lives):

```bash
make
```

This compiles all source files and produces the `arion-lexer` binary at `src/ms1-lexer/arion-lexer`. To clean compiled artifacts:

```bash
make clean
```

---

## Cara Menjalankan Program

### Basic usage

Run directly using the compiled binary:

```bash
./src/ms1-lexer/arion-lexer <input.txt>
```

Token list is printed to the terminal.

### Save output to file

```bash
./src/ms1-lexer/arion-lexer <input.txt> <output.txt>
```

Token list is printed to the terminal **and** written to the specified output file.

### Using the Makefile targets

All `make run*` targets read from `test/milestone-1/` and write output to `test/milestone-1/outputs/` (the folder is created automatically if it does not exist).

```bash
make run                  # test-valid.txt              → outputs/output-valid.txt
make run-invalid          # test-invalid.txt            → outputs/output-invalid.txt
make run-edge             # test-edgecases.txt          → outputs/output-edgecases.txt
make run-caseinsensitive  # test-caseinsensitive.txt    → outputs/output-caseinsensitive.txt
make run-literaltypes     # test-literaltypes.txt       → outputs/output-literaltypes.txt
make run-eql              # test-eql.txt                → outputs/output-eql.txt
make run-kwid             # test-kwidboundary.txt       → outputs/output-kwidboundary.txt
make run-commentambiguity # test-commentambiguity.txt   → outputs/output-commentambiguity.txt
make run-all      # runs all three targets above
```

To remove generated output files:

```bash
make clean-out    # removes test/milestone-1/outputs/*.txt
make clean-all    # removes compiled artifacts + output files
```

### Example

**Input** (`test/milestone-1/test-valid.txt`):

```
program Hello;
var
  a, b: integer;
begin
  a := 5;
end.
```

**Output** (`test/milestone-1/outputs/output-valid.txt`):

```
programsy
ident (Hello)
semicolon
varsy
ident (a)
comma
ident (b)
colon
ident (integer)
semicolon
beginsy
ident (a)
becomes
intcon (5)
semicolon
endsy
period
```

---

## Pembagian Tugas

| NIM      | Pembagian Tugas                                   |
| 13524001 |                                                   |
| 13524027 |                                                   |
| 13524029 |                                                   |
| 13524089 |                                                   |
| 13524093 |                                                   |

The table below records which team member is responsible for each function or class.

### Workspace architecture

| Job                               | PIC      |
| --------------------------------- | -------- |
| Class Definition                  | 13524027 |
| Attributes and methods definition | 13524027 |
| Class cohesion definition         | 13524027 |

### `src/ms1-lexer/token/token.hpp` & `token.cpp`

| Function / Item                            | PIC      |
| ------------------------------------------ | -------- |
| `TokenType` enum definition (all 52 types) | 13524027 |
| `Token` constructor                        | 13524027 |
| `Token::tokenTypeName()`                   | 13524001 |
| `Token::toString()`                        | 13524001 |

### `src/ms1-lexer/dfa/dfa.hpp` & `dfa.cpp`

| Function / Item                                                           | PIC      |
| ------------------------------------------------------------------------- | -------- |
| `State` enum definition                                                   | 13524001 |
| `DFA` constructor                                                         | 13524001 |
| `DFA::advance()`                                                          | 13524001 |
| `DFA::peek()`                                                             | 13524001 |
| `DFA::retract()`                                                          | 13524001 |
| `DFA::skipWhitespace()`                                                   | 13524001 |
| `DFA::isEOF()`                                                            | 13524001 |
| `DFA::nextToken()` — `Q0` dispatch                                        | 13524001 |
| `DFA::nextToken()` — `Q_ALPHA` (identifier/keyword)                       | 13524093 |
| `DFA::nextToken()` — `Q_INT` / `Q_REAL` (number literals)                 | 13524093 |
| `DFA::nextToken()` — `Q_STR` (char/string literals)                       | 13524093 |
| `DFA::nextToken()` — `Q_COLON` / `Q_LT` / `Q_GT` / `Q_EQL` (operators)    | 13524093 |
| `DFA::nextToken()` — `Q_LPAR` / `Q_CMT_BRACE` / `Q_CMT_PAREN*` (comments) | 13524093 |

### `src/ms1-lexer/lexer/symbolTable.hpp` & `symbolTable.cpp`

| Function / Item                         | PIC      |
| --------------------------------------- | -------- |
| `SymbolTable` constructor (keyword map) | 13524089 |
| `SymbolTable::lookup()`                 | 13524089 |

### `src/ms1-lexer/lexer/lexer.hpp` & `lexer.cpp`

| Function / Item      | PIC      |
| -------------------- | -------- |
| `Lexer` constructor  | 13524029 |
| `Lexer::nextToken()` | 13524029 |
| `Lexer::tokenize()`  | 13524029 |
| `Lexer::isEOF()`     | 13524029 |

### `src/ms1-lexer/util/file.hpp` & `file.cpp`

| Function / Item         | PIC      |
| ----------------------- | -------- |
| `FileUtil::readFile()`  | 13524027 |
| `FileUtil::writeFile()` | 13524027 |

### `src/ms1-lexer/main.cpp`

| Function / Item                    | PIC      |
| ---------------------------------- | -------- |
| Argument parsing & file I/O wiring | 13524027 |
| Output formatting                  | 13524029 |

### `src/ms1-lexer/token/token.hpp` & `token.cpp`

| Function / Item | PIC      |
| --------------- | -------- |
| enum TokenType  | 13524027 |
| Token::Token()  | 13524027 |

### Other

| Item                             | PIC                                              |
| -------------------------------- | ------------------------------------------------ |
| DFA transition diagram (draw.io) | 13524001, 13524027, 13524029, 13524089, 13524093 |
| `Makefile`                       | 13524001                                         |
| Test cases (`test/milestone-1/`) | 13524001, 13524089                               |
| Report (`doc/`)                  | 13524001, 13524027, 13524029, 13524089, 13524093 |

---

## Identitas Kelompok

| Kode Kelompok | Nama Kelompok   |
| ------------- | --------------- |
| JBC           | J1B2C2          |

| Nama Anggota             | NIM      |
| ------------------------ | -------- |
| Samuelson D. Tanuraharja | 13524001 |
| Aufa Rienaldifaza Ahmad  | 13524027 |
| Niko Samuel Simanjuntak  | 13524029 |
| Aurelia Jennifer Gunawan | 13524089 |
| Reinsen Silitonga        | 13524093 |
