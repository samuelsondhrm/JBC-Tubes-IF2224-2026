#pragma once
#include "StackMachine.hpp"
#include "../icg/ICInstruction.hpp"
#include <vector>

class Interpreter {
public:
    Interpreter(std::vector<ICInstruction> code);
    void run();

private:
    std::vector<ICInstruction> code_;
    StackMachine vm_;
    int pc_;
    bool running_;

    void executeInstruction(const ICInstruction& instr);
};
