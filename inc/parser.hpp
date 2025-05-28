#pragma once

#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>
#include <initializer_list>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<ASTNode> parse();

private:
    // === Вспомогательные методы для навигации по токенам ===
    void advance();
    Token peek() const;
    Token prev() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types); // NEW: match с несколькими типами
    void expect(TokenType type, const std::string& error_msg);
    void reportError(const std::string& message);
    void synchronize(); // NEW: восстановление после ошибки
    bool isAtEnd() const; // NEW: достигнут ли конец
    bool isType();
    Token peekNext() const;
    


    // === Основные правила грамматики ===
    std::unique_ptr<ASTNode> parseTranslationUnit(); //check
    std::unique_ptr<ASTNode> parseDeclaration();   //check
    std::unique_ptr<VarDeclNode> parseVarDeclaration();     //
    std::unique_ptr<FuncDeclNode> parseFuncDeclaration();
    std::unique_ptr<ASTNode> parseNamespaceDeclaration();
    std::unique_ptr<StructDeclNode> parseStructDeclaration();
    
    // === Операторы (Statements) ===
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    std::unique_ptr<ASTNode> parseBlockStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseLoopStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();

    
    // === Выражения ===
    std::unique_ptr<ASTNode> parseTernary();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseBinary();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    
    
    // === Вспомогательные элементы деклараций ===
    std::unique_ptr<ASTNode> parseType();
    std::unique_ptr<DeclaratorNode> parseDeclarator();
    std::unique_ptr<InitDeclaratorNode> parseInitDeclarator();
    std::unique_ptr<ParamDeclNode> parseParamDeclaration();
    
    // === Состояние парсера ===
    std::vector<Token> tokens;
    size_t current = 0;
    bool hadError = false;
};


/*#pragma once



#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<ASTNode> parse();

private:
    // Вспомогательные методы
    void advance();
    Token peek() const;
    Token prev() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    void expect(TokenType type, const std::string& error_msg);
    void reportError(const std::string& message);

    // Основные правила грамматики
    std::unique_ptr<ASTNode> parseTranslationUnit();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<VarDeclNode> parseVarDeclaration();
    std::unique_ptr<FuncDeclNode> parseFuncDeclaration();
    std::unique_ptr<StructDeclNode> parseStructDeclaration();
    
    // Операторы
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    std::unique_ptr<ASTNode> parseBlockStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseLoopStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    
    // Выражения
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTernaryExpression();
    std::unique_ptr<ASTNode> parseBinaryExpression();
    std::unique_ptr<ASTNode> parseUnaryExpression();
    std::unique_ptr<ASTNode> parsePostfixExpression();
    std::unique_ptr<ASTNode> parsePrimaryExpression();
    
    // Вспомогательные методы для выражений
    std::unique_ptr<ASTNode> parseType();
    std::unique_ptr<DeclaratorNode> parseDeclarator();
    std::unique_ptr<InitDeclaratorNode> parseInitDeclarator();
    std::unique_ptr<ParamDeclNode> parseParamDeclaration();
    
    // Состояние парсера
    std::vector<Token> tokens;
    size_t current = 0;
    bool hadError = false;
};

*/