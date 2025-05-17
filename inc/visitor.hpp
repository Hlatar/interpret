#pragma once

#include "ast.hpp"
#include <vector>

class Visitor {
public:
    virtual void visit(TranslationUnitNode&) = 0;
    // Типы и декларации
    
    virtual void visit(TypeNode&) = 0;
    virtual void visit(DeclaratorNode&) = 0;
    virtual void visit(InitDeclaratorNode&) = 0;
    virtual void visit(VarDeclNode&) = 0;
    virtual void visit(ParamDeclNode&) = 0;
    virtual void visit(FuncDeclNode&) = 0;
    virtual void visit(StructDeclNode&) = 0;
    
    // Операторы
    virtual void visit(BlockStatementNode&) = 0;
    virtual void visit(IfStatementNode&) = 0;
    virtual void visit(WhileLoopNode&) = 0;
    virtual void visit(DoWhileLoopNode&) = 0;
    virtual void visit(ForLoopNode&) = 0;
    virtual void visit(ReturnStatementNode&) = 0;
    virtual void visit(BreakStatementNode&) = 0;
    virtual void visit(ContinueStatementNode&) = 0;
    
    // Выражения
    virtual void visit(BinaryExprNode&) = 0;
    virtual void visit(UnaryExprNode&) = 0;
    virtual void visit(TernaryExprNode&) = 0;
    virtual void visit(CastExprNode&) = 0;
    virtual void visit(SubscriptExprNode&) = 0;
    virtual void visit(CallExprNode&) = 0;
    virtual void visit(AccessExprNode&) = 0;
    virtual void visit(LiteralExprNode&) = 0;
    virtual void visit(IdentifierExprNode&) = 0;
    virtual void visit(GroupExprNode&) = 0;
    virtual void visit(PostfixExprNode&) = 0;
    virtual void visit(InitListNode&) = 0;
    virtual void visit(BreakStmtNode&) = 0;
    virtual void visit(ContinueStmtNode&) = 0;
    virtual void visit(SizeofExprNode&) = 0;
    virtual void visit(StaticAssertNode&) = 0;
    virtual void visit(ExitExprNode&) = 0;
    virtual void visit(AssertExprNode&) = 0;

};
