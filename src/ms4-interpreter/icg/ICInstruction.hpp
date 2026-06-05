#pragma once
#include <string>

struct ICInstruction {
    int line;
    std::string op;
    int level;
    int arg;

    ICInstruction(int line, const std::string& op, int level, int arg)
        : line(line), op(op), level(level), arg(arg) {}
};
