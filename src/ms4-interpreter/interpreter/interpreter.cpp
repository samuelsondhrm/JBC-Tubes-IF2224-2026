#include "interpreter.hpp"
#include <iostream>
#include <stdexcept>

namespace {
    bool is_str(const StackVal& v) { return std::holds_alternative<std::string>(v); }
    bool is_dbl(const StackVal& v) { return std::holds_alternative<double>(v); }

    template<typename T>
    int compare(const T& x, const T& y, int op) {
        if (op == 7) {
            if (x == y) {
                return 1;
            } else {
                return 0;
            }
        } else if (op == 8) {
            if (x != y) {
                return 1;
            } else {
                return 0;
            }
        } else if (op == 9) {
            if (x < y) {
                return 1;
            } else {
                return 0;
            }
        } else if (op == 10) {
            if (x >= y) {
                return 1;
            } else {
                return 0;
            }
        } else if (op == 11) {
            if (x > y) {
                return 1;
            } else {
                return 0;
            }
        } else if (op == 12) {
            if (x <= y) {
                return 1;
            } else {
                return 0;
            }
        } else {
            throw std::runtime_error("Runtime Error: Invalid operator code for comparison: " + std::to_string(op));
        }
    }
}

Interpreter::Interpreter(std::vector<ICInstruction> code)
    : code_(std::move(code)), pc_(0), running_(false) {}

void Interpreter::run() {
    running_ = true;
    pc_ = 0;
    while (running_ && pc_ >= 0 && pc_ < static_cast<int>(code_.size())) {
        executeInstruction(code_[pc_]);
    }
}

void Interpreter::executeInstruction(const ICInstruction& instr) {
    const std::string& op = instr.op;
    int level = instr.level;
    int arg = instr.arg;

    if (op == "LIT") {
        vm_.push(arg);
        pc_++;
    } else if (op == "LOD") {
        vm_.push(vm_.load(level, arg));
        pc_++;
    } else if (op == "STO") {
        vm_.store(level, arg, vm_.pop());
        pc_++;
    } else if (op == "INT") {
        if (vm_.getBP() == 0 && vm_.getSP() < 0) {
            vm_.saveContext(0, (int)code_.size());
        }
        vm_.allocFrame(arg);
        pc_++;
    } else if (op == "JMP") {
        pc_ = arg;
    } else if (op == "JPC") {
        if (toInt(vm_.pop()) == 0) {
            pc_ = arg;
        } else {
            pc_++;
        }
    } else if (op == "CAL") {
        int staticLink = vm_.getBP();
        if (level > 0) {
            int b = vm_.getBP();
            for (int i = 0; i < level; ++i) {
                b = toInt(vm_.at(b + 1));
            }
            staticLink = b;
        }
        vm_.saveContext(staticLink, pc_ + 1);
        pc_ = arg;
    } else if (op == "RET") {
        if (vm_.getBP() == 0) {
            running_ = false;
        } else {
            pc_ = vm_.restoreFrame();
        }
    } else if (op == "OPR") {
        switch (arg) {
            case 1: {
                StackVal val = vm_.pop();
                if (is_dbl(val)) {
                    vm_.push(-toDouble(val));
                } else if (std::holds_alternative<int>(val)) {
                    vm_.push(-toInt(val));
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand type for Negate");
                }
                pc_++;
                break;
            }
            case 2: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (is_str(x) || is_str(y)) {
                    vm_.push(toString(x) + toString(y));
                } else if (is_dbl(x) || is_dbl(y)) {
                    vm_.push(toDouble(x) + toDouble(y));
                } else if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    vm_.push(toInt(x) + toInt(y));
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Add");
                }
                pc_++;
                break;
            }
            case 3: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (is_dbl(x) || is_dbl(y)) {
                    vm_.push(toDouble(x) - toDouble(y));
                } else if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    vm_.push(toInt(x) - toInt(y));
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Subtract");
                }
                pc_++;
                break;
            }
            case 4: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (is_dbl(x) || is_dbl(y)) {
                    vm_.push(toDouble(x) * toDouble(y));
                } else if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    vm_.push(toInt(x) * toInt(y));
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Multiply");
                }
                pc_++;
                break;
            }
            case 5: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (is_dbl(x) || is_dbl(y)) {
                    double dy = toDouble(y);
                    if (dy == 0.0) {
                        throw std::runtime_error("Runtime Error: Division by zero");
                    }
                    vm_.push(toDouble(x) / dy);
                } else if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    int iy = toInt(y);
                    if (iy == 0) {
                        throw std::runtime_error("Runtime Error: Division by zero");
                    }
                    vm_.push(toInt(x) / iy);
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Divide");
                }
                pc_++;
                break;
            }
            case 6: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    int iy = toInt(y);
                    if (iy == 0) {
                        throw std::runtime_error("Runtime Error: Division by zero");
                    }
                    vm_.push(toInt(x) % iy);
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Modulo");
                }
                pc_++;
                break;
            }
            case 7: case 8: case 9: case 10: case 11: case 12: {
                StackVal y = vm_.pop();
                StackVal x = vm_.pop();
                if (is_str(x) || is_str(y)) {
                    vm_.push(compare(toString(x), toString(y), arg));
                } else if (is_dbl(x) || is_dbl(y)) {
                    vm_.push(compare(toDouble(x), toDouble(y), arg));
                } else if (std::holds_alternative<int>(x) && std::holds_alternative<int>(y)) {
                    vm_.push(compare(toInt(x), toInt(y), arg));
                } else {
                    throw std::runtime_error("Runtime Error: Invalid operand types for Comparison");
                }
                pc_++;
                break;
            }
            case 13: {
                std::cout << toString(vm_.pop());
                pc_++;
                break;
            }
            case 14: {
                std::cout << toString(vm_.pop()) << std::endl;
                pc_++;
                break;
            }
            default:
                throw std::runtime_error("Runtime Error: Unknown OPR code: " + std::to_string(arg));
        }
    } else {
        throw std::runtime_error("Runtime Error: Unknown opcode: " + op);
    }
}
