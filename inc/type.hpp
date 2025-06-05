#pragma once

#include <string>
#include <unordered_map>



struct Type {
    virtual ~Type() = default;
    virtual bool isNumeric() const { return false; }
    virtual std::string toString() const = 0;
    virtual bool equals(const Type& other) const = 0;
    
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
    bool equals(const Type& other) const override {
        const auto* o = dynamic_cast<const BuiltinType*>(&other);
        return o && o->name == name && o->is_const == is_const && o->is_unsigned == is_unsigned;
    }
};

struct StructType : public Type {
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<Type>> fields;

    StructType(std::string name) : name(std::move(name)) {}

    void addField(const std::string& fieldName, std::shared_ptr<Type> type) {
        fields[fieldName] = type;
    }

    std::shared_ptr<Type> getFieldType(const std::string& fieldName) const {
        auto it = fields.find(fieldName);
        return it != fields.end() ? it->second : nullptr;
    }

    bool equals(const Type& other) const override {
        auto* o = dynamic_cast<const StructType*>(&other);
        return o && o->name == name;
    }

    std::string toString() const override {
        return "struct " + name;
    }
    ~StructType() override = default;
};
