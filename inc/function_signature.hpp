#pragma once

#include <string>
#include <vector>
#include <memory>
#include "type.hpp"

struct FunctionSignature {
    std::string name;
    std::vector<std::shared_ptr<Type>> paramTypes;
    std::shared_ptr<Type> returnType;
    FunctionSignature() : name(""), paramTypes(), returnType(nullptr) {}

    FunctionSignature(std::string name,
                      std::vector<std::shared_ptr<Type>> params,
                      std::shared_ptr<Type> retType)
        : name(std::move(name)),
          paramTypes(std::move(params)),
          returnType(std::move(retType)) {}

    std::string toString() const;
    bool matches(const std::vector<std::shared_ptr<Type>>& args) const;
};
