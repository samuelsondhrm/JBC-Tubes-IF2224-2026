#pragma once
#include "../symtable/SymbolTable.hpp"
#include <string>

class TypeChecker {
public:
    TypeChecker() = default;

    // cek compatible dari dua tipe (untuk perbandingan, case label, dll)
    bool isCompatible(int typeCode1, int typeCode2) const;

    // cek valueType bisa di-assign ke targetType atau tidak
    bool isAssignmentCompatible(int targetType, int valueType) const;

    // ambil tipe hasil dari operator binary dengan operand kiri dan kanan
    int getOperatorResultType(const std::string& op, int leftType, int rightType) const;

    // cek tipe valid sebagai kondisi boolean (if, while, repeat)
    bool isBooleanType(int typeCode) const;

    // cek tipe termasuk ordinal atau tidak
    bool isOrdinalType(int typeCode) const;

    // cek tipe valid sebagai index array atau tidak
    bool isValidIndexType(int typeCode) const;

    // cek tipe valid sebagai ekspresi case atau tidak
    bool isValidCaseType(int typeCode) const;

    // cek tipe termasuk numerik atau tidak
    bool isNumericType(int typeCode) const;
};
