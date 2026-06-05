# Arion Compiler - Tugas Besar IF2224 Teori Bahasa Formal dan Otomata

> Milestone 4: Intermediate Code Generator & Interpreter

> Milestone 3: Semantic Analyzer

> Milestone 2: Syntax Analyzer

> Milestone 1: Lexical Analyzer

## Daftar Isi
- [Deskripsi Tugas Besar](#deskripsi-tugas-besar)
- [Struktur Project](#struktur-project)
- [Arsitektur](#arsitektur)
- [Persyaratan (Requirements)](#persyaratan-requirements)
- [Instalasi (Setup)](#instalasi-setup)
- [Cara Menjalankan Program](#cara-menjalankan-program)
- [Contoh (Example)](#contoh-example)
- [Pengujian (Testing)](#pengujian-testing)
- [Pembagian Tugas](#pembagian-tugas)
- [Identitas Kelompok](#identitas-kelompok)

## Deskripsi Tugas Besar

### Milestone 1: Lexical Analyzer

Arion Lexer merupakan milestone pertama dari proyek Compiler Arion. Arion lexer adalah sebuah lexical analyzer (lexer) untuk bahasa pemrograman **Arion**, yang diimplementasikan menggunakan C++17 tanpa menggunakan tools maupun library pembangkit lexer otomatis.

Lexer membaca file source code Arion karakter demi karakter dan menghasilkan urutan **token**. Token adalah unit bermakna terkecil dalam sebuah program (misalnya keyword, identifier, operator, dan literal). Proses ini diimplementasikan secara formal sebagai sebuah **Deterministic Finite Automaton (DFA)**, di mana setiap karakter yang dibaca menyebabkan transisi state hingga mencapai accepting state dan menghasilkan sebuah token.

Program menerima file source code berekstensi `.txt` sebagai input dan menghasilkan daftar token sebagai output, baik ditampilkan pada terminal maupun disimpan ke file output tertentu.

### Milestone 2: Syntax Analyzer

Arion Parser merupakan milestone kedua dari proyek Compiler Arion. Arion parser adalah sebuah syntax analyzer (parser) untuk bahasa pemrograman **Arion**, yang diimplementasikan menggunakan algoritma **Recursive Descent** dalam C++17.

Parser menerima hasil tokenisasi dari lexer dan melakukan analisis sintaksis berdasarkan grammar bahasa Arion untuk memastikan bahwa urutan token membentuk struktur program yang valid. Selama proses parsing, parser membangun sebuah **Parse Tree** yang merepresentasikan struktur hierarkis program sesuai aturan grammar yang telah didefinisikan.

Tahapan ini juga mencakup proses **syntax error checking**, di mana parser mendeteksi dan melaporkan kesalahan sintaks dengan pesan error yang informatif apabila ditemukan token yang tidak sesuai dengan grammar.

Program menerima file source code Arion sebagai input, menjalankan proses lexical analysis terlebih dahulu untuk menghasilkan token, kemudian melakukan syntax analysis dan menghasilkan output berupa Parse Tree yang ditampilkan pada terminal maupun disimpan ke dalam file `.txt`.


### Milestone 3: Semantic Analyzer

Arion Semantic Analyzer merupakan milestone ketiga dari proyek Compiler Arion. Arion semantic analyzer adalah sebuah semantic analyzer untuk bahasa pemrograman **Arion**, yang diimplementasikan menggunakan pendekatan **L-Attributed Grammar** dan mekanisme **visitor traversal** pada Abstract Syntax Tree (AST).

Semantic analyzer menerima Parse Tree hasil parser, kemudian mengonversinya menjadi **Abstract Syntax Tree (AST)** yang lebih ringkas dan berfokus pada hubungan semantik antar node program. Setelah itu, dilakukan proses analisis semantik untuk memverifikasi makna program, seperti pengecekan tipe data, validasi deklarasi identifier, manajemen scope, dan kompatibilitas operasi.

Selama proses traversal AST, semantic analyzer menggunakan beberapa **Symbol Table** (`tab`, `btab`, dan `atab`) untuk menyimpan informasi mengenai identifier, block scope, tipe data, array, prosedur, fungsi, serta atribut semantik lainnya. Setiap node AST kemudian dianotasi sehingga menghasilkan sebuah **Decorated AST** yang siap digunakan pada tahap kompilasi berikutnya.

Program menerima file source code Arion sebagai input (menjalankan seluruh pipeline lexer → parser → semantic) dan menghasilkan output berupa Decorated AST beserta Symbol Table yang ditampilkan pada terminal.

### Milestone 4: Intermediate Code Generator & Interpreter

Arion Intermediate Code Generator & Interpreter merupakan milestone keempat sekaligus tahapan akhir dari proyek Compiler Arion. Pada tahap ini, compiler mengonversi **Decorated AST** yang dihasilkan oleh Semantic Analyzer menjadi **Intermediate Code (IC)** berupa instruksi abstrak berbasis *stack machine*, yang kemudian dieksekusi oleh sebuah **Virtual Machine (Interpreter)**.

Intermediate Code Generator menerima Decorated AST yang telah dianotasi dengan informasi semantik, seperti tipe data, scope, dan referensi simbol, kemudian melakukan traversal AST untuk menerjemahkan setiap ekspresi, assignment, percabangan, perulangan, serta pemanggilan prosedur dan fungsi ke dalam serangkaian instruksi intermediate code. Instruksi yang dihasilkan mencakup operasi manipulasi data, operasi aritmetika dan logika, pengendalian alur program, manajemen memori, serta pemanggilan subprogram.

Setelah Intermediate Code terbentuk, Interpreter mengeksekusi instruksi tersebut menggunakan arsitektur **stack machine**. Selama proses eksekusi, interpreter mengelola memori runtime melalui stack dan activation record untuk mendukung lexical scoping, pemanggilan prosedur maupun fungsi, serta eksekusi subprogram bersarang. Interpreter juga bertanggung jawab melakukan evaluasi ekspresi, perpindahan alur kontrol, operasi input-output, dan penanganan berbagai kondisi runtime error.

Program menerima file source code Arion sebagai input, menjalankan seluruh pipeline kompilasi mulai dari lexical analysis, syntax analysis, semantic analysis, hingga intermediate code generation, kemudian menghasilkan output berupa daftar Intermediate Code beserta hasil eksekusi program yang ditampilkan pada terminal.

## Struktur Project

```
JBC-Tubes-IF2224-2026
├── CMakeLists.txt              # Build configuration (CMake)
├── README.md
├── doc/                        # Laporan & diagram
├── cmake/                      # CMake helper scripts (RunDiffTest, RunErrorTest)
├── build/                      # Build output directory (generated)
│   ├── arion-lexer             # Executable MS1
│   ├── arion-parser            # Executable MS2
│   └── arion-semantic          # Executable MS3
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
│   │       ├── file.hpp
│   │       └── file.cpp
│   ├── ms2-parser/
│   │   ├── main.cpp
│   │   ├── ParseNode.cpp
│   │   ├── ParseNode.hpp
│   │   ├── Parser.cpp
│   │   ├── Parser.hpp
│   │   ├── parser_toplevel.cpp
│   │   ├── parser_declarations.cpp
│   │   ├── parser_statements.cpp
│   │   ├── parser_expressions.cpp
│   │   ├── parser_subprograms.cpp
│   │   ├── TreePrinter.cpp
│   │   ├── TreePrinter.hpp
│   ├── ms3-semantic/
│   │   ├── main.cpp
│   │   ├── ast/
│   │   │   └── ASTNode.hpp
│   │   ├── builder/
│   │   │   ├── ASTBuilder.cpp
│   │   │   └── ASTBuilder.hpp
│   │   ├── semantic/
│   │   │   ├── SemanticAnalyzer.cpp
│   │   │   ├── SemanticAnalyzer.hpp
│   │   │   ├── visit_declarations.cpp
│   │   │   ├── visit_expressions.cpp
│   │   │   └── visit_statements.cpp
│   │   ├── symtable/
│   │   │   ├── SymbolTable.cpp
│   │   │   └── SymbolTable.hpp
│   │   ├── type/
│   │   │   ├── TypeChecker.cpp
│   │   │   └── TypeChecker.hpp
│   │   ├── printer/
│   │   │   ├── DecoratedASTPrinter.cpp
│   │   │   └── DecoratedASTPrinter.hpp
│   └── ms4-interpreter/
│       ├── icg/
│       │   ├── ICGen_Expr.cpp
│       │   ├── ICGen_Statements.cpp
│       │   ├── ICGen_Subprograms.cpp
│       │   ├── ICGenerator.hpp
│       │   ├── ICGenerator.cpp
│       │   └── ICInstruction.hpp
│       ├── interpreter/
│       │   ├── interpreter.cpp
│       │   ├── interpreter.hpp
│       │   ├── StackMachine.cpp
│       │   └── StackMachine.hpp
│       └── main.cpp
└── test/
    ├── milestone-1/
    │   ├── test-valid.txt
    │   ├── test-invalid.txt
    │   ├── test-edgecases.txt
    │   ├── ...
    │   └── outputs/
    ├── milestone-2/
    │   ├── test-01-minimal.txt
    │   ├── test-02-vars.txt
    │   ├── ...
    │   ├── expected-errors/
    │   └── outputs/
    ├── milestone-3/
    │   ├── test-01-minimal.txt
    │   ├── test-02-alltypes.txt
    │   ├── ...
    │   ├── run_tests.sh
    │   ├── expected-errors/
    │   └── outputs/
    └── milestone-4/
        ├── tc1_simple_assign.arion
        ├── tc2_if_else.arion
        ├── ...
        └── outputs/
```

### Architecture

Proyek ini dibangun secara bertahap (incremental) sehingga setiap milestone menambahkan satu fase kompilasi baru di atas milestone sebelumnya. Ketiga fase dihubungkan melalui library statis CMake sehingga executable yang lebih tinggi secara otomatis menggunakan komponen milestone sebelumnya.

## Requirements

| Requirement | Version  | Description                                           |
| ----------- | -------- | ----------------------------------------------------- |
| `cmake`     | ≥ 3.16   | Cross-platform build system generator                 |
| `g++`       | ≥ 9.0    | GNU C++ compiler with C++17 support (`-std=c++17`)    |
| `make`      | ≥ 3.81   | Build automation tool (or Ninja, etc.)                |
| OS          | Linux / macOS / WSL | Build & test scripts use standard Unix commands |

No third-party libraries are required. The project uses only the C++17 standard library.

---

## Setup

### 1. Clone the repository

```bash
git clone https://github.com/samuelsondhrm/JBC-Tubes-IF2224-2026.git
cd JBC-Tubes-IF2224-2026
```

### 2. Verify toolchain is available

```bash
cmake --version    # ≥ 3.16
g++ --version      # ≥ 9.0 with C++17
make --version     # ≥ 3.81
```

Jika belum terinstall pada Ubuntu/Debian:

```bash
sudo apt update && sudo apt install build-essential cmake
```

### 3. Build

Dari **project root**, jalankan CMake kemudian compile:

```bash
mkdir -p build
cd build
cmake ..
make
```

Ini akan menghasilkan tiga executable di dalam folder `build/`:
- `arion-lexer` — Lexical Analyzer (Milestone 1)
- `arion-parser` — Syntax Analyzer (Milestone 2)
- `arion-semantic` — Semantic Analyzer (Milestone 3)

Untuk membersihkan hasil build:

```bash
# Dari folder build/
make clean

# Atau hapus seluruh folder build
cd .. && rm -rf build
```

---

## Cara Menjalankan Program

Semua executable dijalankan dari folder `build/`. Setiap executable menerima file source code Arion (`.txt`) sebagai input.

### Milestone 1: Lexer

```bash
# Output ke terminal
./build/arion-lexer <input.txt>

# Output ke terminal + simpan ke file
./build/arion-lexer <input.txt> <output.txt>
```

### Milestone 2: Parser

```bash
# Output ke terminal
./build/arion-parser <input.txt>

# Output ke terminal + simpan ke file
./build/arion-parser <input.txt> <output.txt>
```

### Milestone 3: Semantic Analyzer

```bash
# Output ke terminal
./build/arion-semantic <input.txt>

# Output ke terminal (output file belum didukung; gunakan redirect)
./build/arion-semantic <input.txt> > <output.txt>
```

### Milestone 4: Interpreter

```bash
# Output instruksi IC dan hasil eksekusi ke terminal
./build/arion-interpreter <input.txt>

# Output ke terminal (gunakan redirect untuk simpan file)
./build/arion-interpreter <input.txt> > <output.txt>
```

---

## Example

### Input — Source Code Arion (`test/milestone-1/test-valid.txt`)

```pascal
program Hello;

var
  a, b: integer;

begin
  a := 5;
  b := a + 10;
  writeln('Result = ', b);
end.
```

### Output Milestone 1 — Token List (Lexer)

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
ident (b)
becomes
ident (a)
plus
intcon (10)
semicolon
ident (writeln)
lparent
string ('Result = ')
comma
ident (b)
rparent
semicolon
endsy
period
```

### Output Milestone 2 — Parse Tree (Parser)

Input (`test/milestone-2/test-01-minimal.txt`):
```pascal
program Hello;
begin
end.
```

Output:
```
<program>
├── <program-header>
│   ├── programsy
│   ├── ident(Hello)
│   └── semicolon
├── <declaration-part>
├── <compound-statement>
│   ├── beginsy
│   ├── <statement-list>
│   └── endsy
└── period
```

### Output Milestone 3 — Decorated AST + Symbol Tables (Semantic)

Input (`test/milestone-3/test-01-minimal.txt`):
```pascal
program Minimal;
begin
end.
```

Output:
```
└── Program('Minimal') → tab:8, type:8
    └── Compound

=== TAB (Symbol Table) ===
 idx name             obj  typ  ref  nrm  lev  adr link
   1 Real               2    2    0    1    0    0   -1
   2 Integer            2    1    0    1    0    0    1
   3 Char               2    4    0    1    0    0    2
   4 Boolean            2    3    0    1    0    0    3
   5 String             2    5    0    1    0    0    4
   6 True               1    3    0    1    0    1    5
   7 False              1    3    0    1    0    0    6
   8 Minimal            5    8    0    1    0    0   -1

=== BTAB (Block Table) ===
 idx   last   lpar  psize  vsize
   0      7      0      0      0
   1      8      0      0      0

=== ATAB (Array Table) ===
 idx   xtyp   etyp   eref    low   high   elsz   size
```

### Output Milestone 4 — Intermediate Code & Execution Result (Interpreter)

Input (`test/milestone-4/tc1_simple_assign.arion`):
```pascal
program Test;
var x: integer;
begin
  x := 5;
  writeln(x)
end.
```

Output:
```
0: INT 0 4
1: LIT 0 5
2: STO 0 3
3: LOD 0 3
4: OPR 0 14
5: RET 0 0
5
```

---

## Testing

Proyek ini menggunakan **CTest** (bawaan CMake) untuk automated testing. Seluruh test case didefinisikan di `CMakeLists.txt`.

### Menjalankan semua test

```bash
cd build
ctest --output-on-failure
```

### Menjalankan test untuk milestone tertentu

```bash
# Hanya test Milestone 1
ctest -R ms1 --output-on-failure

# Hanya test Milestone 2
ctest -R ms2 --output-on-failure

# Hanya test Milestone 3
ctest -R ms3 --output-on-failure

# Hanya test Milestone 4
ctest -R ms4 --output-on-failure
```

### Script test khusus Milestone 3

Tersedia shell script tambahan untuk menjalankan test Milestone 3 secara lebih detail:

```bash
bash test/milestone-3/run_tests.sh
```

Script ini menjalankan semua test case valid (test-01 s/d test-06) dan error (test-07 s/d test-16), kemudian menampilkan ringkasan hasil pass/fail.

---

## Pembagian Tugas

| Milestone   | NIM      | Pembagian Tugas                                                                |
|-------------|----------|--------------------------------------------------------------------------------|
|  1 | 13524001 | Membuat class DFA, merangkai DFA model untuk pembacaan intcon, realcon, charcon, string, dan menyusun laporan            |
|             | 13524027 | Implementasi I/O file, string parsing, dan class token, merangkai DFA model untuk logical operators, parentheses, brackets, dan comment, menyusun laporan                      |
|             | 13524029 | Implementasi class lexer, merangkai DFA model untuk pembacaan simbol ident, proceduresy, programsy, notsy, orsy, ofsy, typesy, thensy, tosy, recordsy, repeatsy, varsy, whilesy, dan untilsy, menyusun laporan |
|             | 13524089 | Implementasi symbol table, merangkai DFA model untuk pembacaan simbol lss, leq, neq, gtr, geq, eql, becomes, dan colon, menyusun laporan                 |
|             | 13524093 | Implementasi class DFA, merangkai DFA model untuk pembacaan andsy, arraysy, beginsy, casesy, constsy, downtosy, elsesy, endsy, forsy, functionsy, ifsy, idiv, dan imod, menyusun laporan                    |
|  2 | 13524001 | Implementasi top-level grammar, dan menyusun laporan                  |
|             | 13524027 | Implementasi kelas parser, parseNode, parseError, dan menyusun laporan                 |
|             | 13524029 | Implementasi declarations grammar, dan menyusun laporan                      |
|             | 13524089 | Implementasi statements grammar, dan menyusun laporan                       |
|             | 13524093 | Implementasi expressions grammar, subprograms, dan menyusun laporan                 |
|  3 | 13524001 | Implementasi ASTNode, ASTBuilder, DecoratedASTPrinter, dan menyusun laporan                                    |
|             | 13524027 | Implementasi SymbolTable, dan menyusun laporan                          |
|             | 13524029 | Implementasi visit_declarations, dan menyusun laporan                    |
|             | 13524089 | Implementasi visit_expressions dan visit_statement, dan menyusun laporan                             |
|             | 13524093 | Implementasi TypeChecker, dan menyusun laporan                        |
|  4 | 13524001 | Implementasi ICGen_Expr, dan menyusun laporan                                    |
|             | 13524027 | Implementasi interpreter, dan menyusun laporan                          |
|             | 13524029 | Implementasi ICGen_Statements, ICGen_Subprograms, dan menyusun laporan                    |
|             | 13524089 | Implementasi StackMachine, dan menyusun laporan                             |
|             | 13524093 | Implementasi ICGenerator, ICInstruction dan menyusun laporan                        |

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
