#include "function_signature.hpp"

std::string FunctionSignature::toString() const {
    std::string result = returnType->toString() + " " + name + "(";
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        result += paramTypes[i]->toString();
        if (i + 1 < paramTypes.size()) result += ", ";
    }
    result += ")";
    return result;
}

bool FunctionSignature::matches(const std::vector<std::shared_ptr<Type>>& args) const {
    if (args.size() != paramTypes.size()) return false;
    for (size_t i = 0; i < args.size(); ++i) {
        if (paramTypes[i]->toString() != args[i]->toString()) {
            return false;
        }
    }
    return true;
}
