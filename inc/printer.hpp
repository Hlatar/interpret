#pragma once
#include "visitor.hpp"
#include "ast.hpp"
#include <iostream>

class PrintVisitor : public Visitor {
public:
    int indent_level = 0;
    void indent() const ;
    void print(class ASTNode&);
    // Перегрузка методов для каждого типа узла
    void visit(class TranslationUnitNode&) override;
    void visit(class TypeNode&) override;
    void visit(class DeclaratorNode&) override;
    void visit(class InitDeclaratorNode&) override;
    void visit(class VarDeclNode&) override;
    void visit(class ParamDeclNode&) override;
    void visit(class FuncDeclNode&) override;
    void visit(class StructDeclNode&) override;
     
    void visit(class BlockStatementNode&) override;
    void visit(class IfStatementNode&) override;
    void visit(class WhileLoopNode&) override;
    void visit(class DoWhileLoopNode&) override;
    void visit(class ForLoopNode&) override;
    void visit(class ReturnStatementNode&) override;
 
    void visit(class BinaryExprNode&) override;
    void visit(class UnaryExprNode&) override;
    void visit(class TernaryExprNode&) override;
    void visit(class CastExprNode&) override;
    void visit(class SubscriptExprNode&) override;
    void visit(class CallExprNode&) override;
    void visit(class LiteralExprNode&) override;
    void visit(class IdentifierExprNode&) override;
    void visit(class GroupExprNode&) override;
    void visit(class PostfixExprNode&) override;
    void visit(class InitListNode&) override;
    void visit(class BreakStmtNode&) override;
    void visit(class ContinueStmtNode&) override;
    void visit(class SizeofExprNode&) override;
    void visit(class StaticAssertNode&) override;
    void visit(class ExitExprNode&) override;
    void visit(class AssertExprNode&) override;
    void visit(class ReadStmtNode&) override;
    void visit(class PrintStmtNode&) override;
    void visit(class AssignmentExprNode&) override;
    void visit(class MemberAccessExprNode&) override;
    void visit(class NamespaceDeclNode&) override;
    void visit(class ScopedIdentifierExprNode&) override;
    
    
private:
    // Вспомогательные методы для печати или форматирования
    void printIndentation(int level);
    void printNodeName(const std::string& name);
};


