#include "StackMachine.hpp"

// inisialisasi stack ke keadaan kosong dan alokasi seluruh array
StackMachine::StackMachine() : sp_(-1), bp_(0) {
    stack_.resize(MAX_STACK);
}

// push nilai
void StackMachine::push(StackVal val) {
    if (sp_ >= MAX_STACK - 1) {
        throw std::runtime_error("Runtime Error: Stack Overflow");
    }
    stack_[++sp_] = std::move(val);
}


// pop nilai
StackVal StackMachine::pop() {
    if (sp_ < 0) {
        throw std::runtime_error("Runtime Error: Stack Underflow");
    }
    return stack_[sp_--];
}

// baca nilai yang ada di puncak stack
StackVal StackMachine::top() const {
    if (sp_ < 0) {
        throw std::runtime_error("Runtime Error: Stack Underflow");
    }
    return stack_[sp_];
}

//cek apakah stack kosong
bool StackMachine::empty() const {
    return sp_ < 0;
}


// menyimpan konteks eksekusi sebelum masuk ke subprogram baru
void StackMachine::saveContext(int paramCount, bool hasReturnSlot, int staticLink, int returnAddr) {
    if (paramCount < 0) {
        throw std::runtime_error("Runtime Error: Invalid parameter count");
    }

    int headerSize = 3 + (hasReturnSlot ? 1 : 0);
    int argStart = sp_ - paramCount + 1;

    if (argStart < 0) {
        throw std::runtime_error("Runtime Error: Not enough arguments on stack");
    }

    if (sp_ + headerSize >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Stack Overflow");
    }

    for (int i = sp_; i >= argStart; --i) {
        stack_[i + headerSize] = std::move(stack_[i]);
    }

    int oldBP = bp_;
    bp_ = argStart;

    stack_[bp_ + 0] = static_cast<int>(staticLink);
    stack_[bp_ + 1] = static_cast<int>(oldBP);
    stack_[bp_ + 2] = static_cast<int>(returnAddr);

    if (hasReturnSlot) {
        stack_[bp_ + 3] = 0;
    }

    sp_ += headerSize;
}

// alokasi sisa slot frame setelah saveContext mendorong 3 slot header
void StackMachine::allocFrame(int n) {
    if (n < 3) {
        throw std::runtime_error("Runtime Error: Invalid frame size (must be >= 3)");
    }

    int targetSP = bp_ + n - 1;

    if (targetSP >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Stack Overflow");
    }

    if (targetSP > sp_) {
        sp_ = targetSP;
    }
}


// memulihkan registers ke frame pemanggil setelah RET dieksekusi
int StackMachine::restoreFrame() {
    int retAddr = toInt(stack_[bp_ + 2]);
    int oldBP = toInt(stack_[bp_ + 1]);
    sp_ = bp_ - 1;
    bp_ = oldBP;
    return retAddr;
}

// resolve base pointer untuk lexical level tertentu 
int StackMachine::base(int level) const {
    int b = bp_;
    for (int i = 0; i < level; ++i) {
        if (b < 0 || b + 1 >= MAX_STACK) {
            throw std::runtime_error("Runtime Error: Invalid frame chain in base()");
        }
        b = toInt(stack_[b + 1]);
    }
    return b;
}

// baca nilai dari frame pada lexical level level, di offset addr dari base pointer level
StackVal StackMachine::load(int level, int addr) {
    int b = base(level);
    int idx = b + addr;
    if (idx < 0 || idx >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Memory Access Out of Bounds (LOD)");
    }
    return stack_[idx];
}


// menyimpan nilai ke frame pada lexical level level, di offset addr dari base pointer level
void StackMachine::store(int level, int addr, StackVal val) {
    int b = base(level);
    int idx = b + addr;
    if (idx < 0 || idx >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Memory Access Out of Bounds (STO)");
    }
    stack_[idx] = std::move(val);
}

// akses langsung slot stack berdasarkan indeks absolut
StackVal& StackMachine::at(int index) {
    if (index < 0 || index >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Memory Access Out of Bounds");
    }
    return stack_[index];
}

// versi const dari at() untuk keamanan saat akses stack
const StackVal& StackMachine::at(int index) const {
    if (index < 0 || index >= MAX_STACK) {
        throw std::runtime_error("Runtime Error: Memory Access Out of Bounds");
    }
    return stack_[index];
}