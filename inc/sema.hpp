#pragma once

#include "scope.hpp"
#include "function_signature.hpp"
#include "ast.hpp" // Заглушка: предполагается, что есть ASTNode и FunctionDeclNode
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <string>
#include "visitor.hpp"

class SemanticError : public std::runtime_error {
public:
    explicit SemanticError(const std::string& message)
        : std::runtime_error("Semantic Error: " + message) {}

    // Конструктор с информацией о позиции в исходном коде (если поддерживается)
    SemanticError(const std::string& message, int line, int column)
        : std::runtime_error("Semantic Error at line " + std::to_string(line) + 
                             ", column " + std::to_string(column) + ": " + message) {}

    // Можно добавить дополнительные поля, например, тип ошибки
    // enum class ErrorType { TypeMismatch, UndeclaredVariable, Redefinition, ... };
    // ErrorType type;
};

class SemanticAnalyzer : public Visitor {
public:
    void analyze(ASTNode& root); // check
    void visit( TranslationUnitNode& node) override;// check
    void visit( TypeNode& node) override;
    void visit( DeclaratorNode& node) override;
    void visit( InitDeclaratorNode& node) override;
    void visit( VarDeclNode& node) override; // check
    void visit( ParamDeclNode& node) override;
    void visit( FuncDeclNode& node) override; // check
    void visit( StructDeclNode& node) override;
     
    void visit( BlockStatementNode& node) override;
    void visit( IfStatementNode& node) override;
    void visit( WhileLoopNode& node) override;
    void visit( DoWhileLoopNode& node) override;
    void visit( ForLoopNode& node) override;
    void visit( ReturnStatementNode& node) override;
 
    void visit( BinaryExprNode& node) override;
    void visit( UnaryExprNode& node) override;
    void visit( TernaryExprNode& node) override;
    void visit( CastExprNode& node) override;
    void visit( SubscriptExprNode& node) override;
    void visit( CallExprNode& node) override;
    void visit( LiteralExprNode& node) override;
    void visit( IdentifierExprNode& node) override;
    void visit( GroupExprNode& node) override;
    void visit( PostfixExprNode& node) override;
    void visit( InitListNode& node) override;
    void visit( BreakStmtNode& node) override;
    void visit( ContinueStmtNode& node) override;
    void visit( SizeofExprNode& node) override;
    void visit( StaticAssertNode& node) override;
    void visit( ExitExprNode& node) override;
    void visit( AssertExprNode& node) override;
    void visit( ReadStmtNode& node) override;
    void visit( PrintStmtNode& node) override;
    void visit( AssignmentExprNode& node) override;
    void visit( MemberAccessExprNode& node) override;
    void visit( NamespaceDeclNode& node) override;
    void visit( ScopedIdentifierExprNode& node) override;
private:
    std::shared_ptr<Scope> currentScope;
    std::unordered_map<std::string, FunctionSignature> functionTable;
    std::unordered_map<std::string, std::shared_ptr<Type>> typeTable;
    std::vector<std::shared_ptr<Type>> expectedReturnTypes;

    void enterScope() {
        currentScope = std::make_shared<Scope>(currentScope);
    }

    void exitScope() {
        if (currentScope) currentScope = currentScope->getParent();
    }

    bool declareVariable(const std::string& name, const std::shared_ptr<Type>& type) {
        return currentScope && currentScope->declare(name, type);
    }

    std::optional<std::shared_ptr<Type>> lookupVariable(const std::string& name) const {
        return currentScope ? currentScope->lookup(name) : std::nullopt;
    }

    void collectFunctionSignatures(TranslationUnitNode& node);
    

    std::vector<std::shared_ptr<Type>> extractTypes(const std::vector<std::pair<std::string, std::shared_ptr<Type>>>& params) {
        std::vector<std::shared_ptr<Type>> result;
        for (const auto& p : params)
            result.push_back(p.second);
        return result;
    }
    std::shared_ptr<Type> getType(ASTNode& expr);
};
