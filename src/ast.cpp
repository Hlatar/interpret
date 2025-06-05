#include "../inc/ast.hpp"
#include "../inc/visitor.hpp"

// Translation Unit
void TranslationUnitNode::accept(Visitor& visitor) {
    visitor.visit(*this);
  // Optionally implement if you have visit(TranslationUnitNode&) in Visitor
}

// Type & Declarations
void TypeNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void DeclaratorNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void InitDeclaratorNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void VarDeclNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ParamDeclNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FuncDeclNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void StructDeclNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

// Statements
void BlockStatementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void IfStatementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void DoWhileLoopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ForLoopNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

// BreakStatementNode and ContinueStatementNode use empty accept() inline in the header

// Expressions
void BinaryExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void UnaryExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void TernaryExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CastExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void SubscriptExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CallExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void LiteralExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void IdentifierExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void GroupExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void PostfixExprNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void InitListNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void BreakStmtNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void ContinueStmtNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void SizeofExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void StaticAssertNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void ExitExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void AssertExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void ReadStmtNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void PrintStmtNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void AssignmentExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void MemberAccessExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void NamespaceDeclNode::accept(Visitor& visitor){
    visitor.visit(*this);
}

void ScopedIdentifierExprNode::accept(Visitor& visitor){
    visitor.visit(*this);
}






