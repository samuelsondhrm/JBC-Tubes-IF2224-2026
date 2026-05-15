#include "TypeChecker.hpp"

namespace {

bool isIntegerBased(int typeCode) {
    return typeCode == TYPE_INTEGER || typeCode == TYPE_SUBRANGE;
}

} // namespace

bool TypeChecker::isCompatible(int typeCode1, int typeCode2) const {
    if (typeCode1 == typeCode2) {
        return true;
    }

    if (isIntegerBased(typeCode1) && isIntegerBased(typeCode2)) {
        return true;
    }

    return false;
}

bool TypeChecker::isAssignmentCompatible(int targetType, int valueType) const {
    if (targetType == TYPE_REAL && valueType == TYPE_INTEGER) {
        return true;
    }

    if (targetType == TYPE_REAL && valueType == TYPE_SUBRANGE) {
        return true;
    }

    return isCompatible(targetType, valueType);
}

int TypeChecker::getOperatorResultType(const std::string& op, int leftType, int rightType) const {
    if (op == "+" || op == "-" || op == "*") {
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            return TYPE_NONE;
        }
        if (leftType == TYPE_REAL || rightType == TYPE_REAL) {
            return TYPE_REAL;
        }
        return TYPE_INTEGER;
    }

    if (op == "/") {
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            return TYPE_NONE;
        }
        return TYPE_REAL;
    }

    if (op == "div" || op == "mod") {
        if (!isIntegerBased(leftType) || !isIntegerBased(rightType)) {
            return TYPE_NONE;
        }
        return TYPE_INTEGER;
    }

    if (op == "and" || op == "or") {
        if (leftType != TYPE_BOOLEAN || rightType != TYPE_BOOLEAN) {
            return TYPE_NONE;
        }
        return TYPE_BOOLEAN;
    }

    if (op == "==" || op == "=" || op == "<>") {
        if (isCompatible(leftType, rightType)) {
            return TYPE_BOOLEAN;
        }
        if (isNumericType(leftType) && isNumericType(rightType)) {
            return TYPE_BOOLEAN;
        }
        return TYPE_NONE;
    }

    if (op == "<" || op == ">" || op == "<=" || op == ">=") {
        if (leftType == TYPE_BOOLEAN || leftType == TYPE_ARRAY || leftType == TYPE_RECORD) {
            return TYPE_NONE;
        }
        if (rightType == TYPE_BOOLEAN || rightType == TYPE_ARRAY || rightType == TYPE_RECORD) {
            return TYPE_NONE;
        }
        if (isCompatible(leftType, rightType)) {
            return TYPE_BOOLEAN;
        }
        if (isNumericType(leftType) && isNumericType(rightType)) {
            return TYPE_BOOLEAN;
        }
        return TYPE_NONE;
    }

    return TYPE_NONE;
}

bool TypeChecker::isBooleanType(int typeCode) const {
    return typeCode == TYPE_BOOLEAN;
}

bool TypeChecker::isOrdinalType(int typeCode) const {
    return typeCode == TYPE_INTEGER ||
           typeCode == TYPE_CHAR ||
           typeCode == TYPE_BOOLEAN ||
           typeCode == TYPE_SUBRANGE;
}

bool TypeChecker::isValidIndexType(int typeCode) const {
    return isOrdinalType(typeCode);
}

bool TypeChecker::isValidCaseType(int typeCode) const {
    return typeCode == TYPE_INTEGER ||
           typeCode == TYPE_CHAR ||
           typeCode == TYPE_BOOLEAN ||
           typeCode == TYPE_SUBRANGE;
}

bool TypeChecker::isNumericType(int typeCode) const {
    return typeCode == TYPE_INTEGER ||
           typeCode == TYPE_REAL ||
           typeCode == TYPE_SUBRANGE;
}
