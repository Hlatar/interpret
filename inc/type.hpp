#pragma once

#include <string>


struct Type {
    virtual ~Type() = default;
    virtual bool isNumeric() const { return false; }
    virtual std::string toString() const = 0;
};

struct BuiltinType : public Type {
    std::string name;
    bool is_const;
    bool is_unsigned;
    BuiltinType(std::string name, bool is_const = false, bool is_unsigned = false)
        : name(std::move(name)), is_const(is_const), is_unsigned(is_unsigned) {}

    bool isNumeric() const override {
        return name == "int" || name == "float" || name == "double";
    }

    std::string toString() const override {
        return name;
    }
};
