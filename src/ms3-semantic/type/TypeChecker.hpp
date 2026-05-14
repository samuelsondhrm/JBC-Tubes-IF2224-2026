#pragma once
#include <string>

class TypeChecker {
public:
    // Check if two types are compatible
    bool isCompatible(int typeA, int typeB);

    // Check if a value type can be assigned to a target type
    bool isAssignmentCompatible(int targetType, int valueType);

    // Get the result type of a binary operator applied to left/right operand types
    int getOperatorResultType(const std::string& op, int leftType, int rightType);
};
