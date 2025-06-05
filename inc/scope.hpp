#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>
#include "type.hpp" // Подключи свой тип переменных

class Scope {
public:
    explicit Scope(std::shared_ptr<Scope> parent = nullptr);

    // Добавляет переменную в текущую область видимости
    bool declare(const std::string& name, const std::shared_ptr<Type>& type);

    // Ищет переменную во всех родительских областях
    std::optional<std::shared_ptr<Type>> lookup(const std::string& name) const;

    // Только локальная проверка (без родителей)
    bool isDeclaredLocally(const std::string& name) const;

    std::shared_ptr<Scope> getParent() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Type>> variables;
    std::shared_ptr<Scope> parent;
};
