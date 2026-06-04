#pragma once
#include <vector>
#include <stdexcept>
#include <string>
#include <variant>

using StackVal = std::variant<int, double, std::string>; // tipe nilai yg bs disimpan di stack

// ambil nilai sebagai int dari StackVal
inline int toInt(const StackVal& v) {
    if (std::holds_alternative<int>(v)){
        return std::get<int>(v);
    }
    if (std::holds_alternative<double>(v)){
        return static_cast<int>(std::get<double>(v));
    }
    return 0;
}

// ambil nilai sebagai double dari StackVal
inline double toDouble(const StackVal& v) {
    if (std::holds_alternative<double>(v)){
        return std::get<double>(v);
    }
    if (std::holds_alternative<int>(v)){
        return static_cast<double>(std::get<int>(v));
    }
    return 0.0;
}

// ubah StackVal menjadi string
inline std::string toString(const StackVal& v) {
    if (std::holds_alternative<std::string>(v)){
        return std::get<std::string>(v);
    }
    if (std::holds_alternative<int>(v)){
        return std::to_string(std::get<int>(v));
    }
    if (std::holds_alternative<double>(v)){
        std::string s = std::to_string(std::get<double>(v));
        // hapus trailing 0s setelah desimal
        s.erase(s.find_last_not_of('0') + 1);
        if (!s.empty() && s.back() == '.') s.pop_back();
        return s;
    }
    return "";
}

// kelas yang merepresentasikan komponen memori eksekusi untuk interpreter
class StackMachine {
public:
    static constexpr int MAX_STACK = 10000;

    StackMachine();

    void push(StackVal val);
    StackVal pop();
    StackVal top() const;
    bool empty() const;
    void saveContext(int paramCount, bool hasReturnSlot, int staticLink, int returnAddr);
    void allocFrame(int n);
    int restoreFrame();
    StackVal load(int level, int addr);
    void store(int level, int addr, StackVal val);
    StackVal& at(int index);
    const StackVal& at(int index) const;

    int getBP() const{
        return bp_; 
    }
    int getSP() const{
        return sp_; 
    }
    void setBP(int val){ 
        bp_ = val; 
    }
    void setSP(int val){ 
        sp_ = val; 
    }

    int base(int level) const;

private:
    std::vector<StackVal> stack_;
    int sp_;
    int bp_;
};