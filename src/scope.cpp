#include "../inc/scope.hpp"

Scope::Scope(std::shared_ptr<Scope> parent)
    : parent(std::move(parent)) {}

bool Scope::declare(const std::string& name, const std::shared_ptr<Type>& type) {
    if (variables.count(name)) return false; // Уже есть локально
    variables[name] = type;
    return true;
}

std::optional<std::shared_ptr<Type>> Scope::lookup(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) return it->second;

    if (parent) return parent->lookup(name);
    return std::nullopt;
}

bool Scope::isDeclaredLocally(const std::string& name) const {
    return variables.count(name) > 0;
}

std::shared_ptr<Scope> Scope::getParent() const {
    return parent;
}