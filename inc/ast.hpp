#pragma once

#include <string>
#include <vector>
#include <memory>


struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(class Visitor&) = 0;
};

struct TranslationUnitNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> declarations;

    TranslationUnitNode(std::vector<std::unique_ptr<ASTNode>> decls)
        : declarations(std::move(decls)) {}

    void accept(Visitor&) override;
};

struct TypeNode : ASTNode {
    std::string type_name;
    bool is_const;

    TypeNode(const std::string& name, bool is_const = false)
        : type_name(name), is_const(is_const) {}

    void accept(Visitor&) override;
};

struct DeclaratorNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> array_size;

    DeclaratorNode(const std::string& name, std::unique_ptr<ASTNode> size = nullptr)
        : name(name), array_size(std::move(size)) {}

    void accept(Visitor&) override;
};

struct InitDeclaratorNode : ASTNode {
    std::unique_ptr<DeclaratorNode> declarator;
    std::unique_ptr<ASTNode> initializer;

    InitDeclaratorNode(std::unique_ptr<DeclaratorNode> decl, std::unique_ptr<ASTNode> init = nullptr)
        : declarator(std::move(decl)), initializer(std::move(init)) {}

    void accept(Visitor&) override;
};


struct InitListNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
    InitListNode(std::vector<std::unique_ptr<ASTNode>> elements) : elements(std::move(elements)) {}
    void accept(Visitor&) override;
};



struct VarDeclNode : ASTNode {
    std::unique_ptr<ASTNode> type;
    std::vector<std::unique_ptr<InitDeclaratorNode>> declarators;
    bool is_const;

    VarDeclNode(std::unique_ptr<ASTNode> type, std::vector<std::unique_ptr<InitDeclaratorNode>> decls, bool is_const = false)
        : type(std::move(type)), declarators(std::move(decls)), is_const(is_const) {}

    void accept(Visitor&) override;
};

struct ParamDeclNode : ASTNode {
    std::unique_ptr<ASTNode> type;
    std::unique_ptr<DeclaratorNode> declarator;
    bool is_const;

    ParamDeclNode(std::unique_ptr<ASTNode> type, std::unique_ptr<DeclaratorNode> declarator, bool is_const = false)
        : type(std::move(type)), declarator(std::move(declarator)), is_const(is_const) {}

    void accept(Visitor&) override;
};

struct FuncDeclNode : ASTNode {
    std::unique_ptr<ASTNode> return_type;
    std::string name;
    std::vector<std::unique_ptr<ParamDeclNode>> params;
    std::unique_ptr<ASTNode> body;
    bool is_const;

    FuncDeclNode(std::unique_ptr<ASTNode> ret, const std::string& name,
                 std::vector<std::unique_ptr<ParamDeclNode>> params,
                 std::unique_ptr<ASTNode> body = nullptr, bool is_const = false)
        : return_type(std::move(ret)), name(name),
          params(std::move(params)), body(std::move(body)), is_const(is_const) {}

    void accept(Visitor&) override;
};

struct StructDeclNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<VarDeclNode>> members;

    StructDeclNode(const std::string& name, std::vector<std::unique_ptr<VarDeclNode>> members)
        : name(name), members(std::move(members)) {}

    void accept(Visitor&) override;
};

struct BreakStmtNode : ASTNode{
    BreakStmtNode() = default;
    void accept(Visitor& visitor) override ;
};

struct ContinueStmtNode : ASTNode {
    ContinueStmtNode() = default;
    void accept(Visitor& visitor) override ;
};

struct SizeofExprNode : ASTNode {
    std::unique_ptr<ASTNode> operand;
    bool isType; // sizeof(type) , sizeof expr ;
    SizeofExprNode(std::unique_ptr<ASTNode> op, bool isType) : operand(std::move(op)), isType(isType) {}
    void accept(Visitor& visitor) override;
};

struct StaticAssertNode : ASTNode {
    std::unique_ptr<ASTNode> condition; // ExprNode
    std::string message; // Строковый литерал (может быть пустым)
    StaticAssertNode(std::unique_ptr<ASTNode> cond, std::string msg)
        : condition(std::move(cond)), message(std::move(msg)) {}
    void accept(Visitor& visitor) override;
};

struct ExitExprNode : ASTNode  {
    std::vector<std::unique_ptr<ASTNode>> arguments;
    ExitExprNode( std::vector<std::unique_ptr<ASTNode>> args)
        : arguments(std::move(args)) {}
    void accept(Visitor& visitor) override;
};

struct AssertExprNode : ASTNode  {
    std::vector<std::unique_ptr<ASTNode>> arguments;
    AssertExprNode( std::vector<std::unique_ptr<ASTNode>> args)
        : arguments(std::move(args)) {}
    void accept(Visitor& visitor) override;
};


struct BlockStatementNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;

    BlockStatementNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)) {}

    void accept(Visitor&) override;
};

struct IfStatementNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;

    IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then_b, std::unique_ptr<ASTNode> else_b = nullptr)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}

    void accept(Visitor&) override;
};

struct WhileLoopNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileLoopNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body)
        : condition(std::move(cond)), body(std::move(body)) {}

    void accept(Visitor&) override;
};

struct DoWhileLoopNode : ASTNode {
    std::unique_ptr<ASTNode> body;
    std::unique_ptr<ASTNode> condition;

    DoWhileLoopNode(std::unique_ptr<ASTNode> body, std::unique_ptr<ASTNode> cond)
        : body(std::move(body)), condition(std::move(cond)) {}

    void accept(Visitor&) override;
};

struct ForLoopNode : ASTNode {
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;

    ForLoopNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond,
                std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> body)
        : init(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(body)) {}

    void accept(Visitor&) override;
};

struct ReturnStatementNode : ASTNode {
    std::unique_ptr<ASTNode> expression;

    ReturnStatementNode(std::unique_ptr<ASTNode> expr = nullptr)
        : expression(std::move(expr)) {}

    void accept(Visitor&) override;
};

struct BreakStatementNode : ASTNode {
    void accept(Visitor&) override {}
};

struct ContinueStatementNode : ASTNode {
    void accept(Visitor&) override {}
};

struct BinaryExprNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryExprNode(const std::string& op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}

    void accept(Visitor&) override;
};

struct UnaryExprNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> operand;

    UnaryExprNode(const std::string& op, std::unique_ptr<ASTNode> operand)
        : op(op), operand(std::move(operand)) {}

    void accept(Visitor&) override;
};

struct TernaryExprNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_expr;
    std::unique_ptr<ASTNode> else_expr;

    TernaryExprNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then_e, std::unique_ptr<ASTNode> else_e)
        : condition(std::move(cond)), then_expr(std::move(then_e)), else_expr(std::move(else_e)) {}

    void accept(Visitor&) override;
};

struct CastExprNode : ASTNode {
    std::unique_ptr<ASTNode> type;
    std::unique_ptr<ASTNode> expression;

    CastExprNode(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> expr)
        : type(std::move(type)), expression(std::move(expr)) {}

    void accept(Visitor&) override;
};

struct SubscriptExprNode : ASTNode {
    std::unique_ptr<ASTNode> array;
    std::unique_ptr<ASTNode> index;

    SubscriptExprNode(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> index)
        : array(std::move(array)), index(std::move(index)) {}

    void accept(Visitor&) override;
};

struct CallExprNode : ASTNode {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    CallExprNode(std::unique_ptr<ASTNode> callee, std::vector<std::unique_ptr<ASTNode>> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}

    void accept(Visitor&) override;
};

struct AccessExprNode : ASTNode {
    std::unique_ptr<ASTNode> object;
    std::string member;
    bool is_pointer_access;

    AccessExprNode(std::unique_ptr<ASTNode> object, const std::string& member, bool is_ptr)
        : object(std::move(object)), member(member), is_pointer_access(is_ptr) {}

    void accept(Visitor&) override;
};

struct LiteralExprNode : ASTNode {
    std::unique_ptr<ASTNode> type;
    std::string value;

    LiteralExprNode(std::unique_ptr<ASTNode> type, const std::string& value)
        : type(std::move(type)), value(value) {}

    void accept(Visitor&) override;
};

struct IdentifierExprNode : ASTNode {
    std::string name;

    IdentifierExprNode(const std::string& name)
        : name(name) {}

    void accept(Visitor&) override;
};

struct GroupExprNode : ASTNode {
    std::unique_ptr<ASTNode> expression;

    GroupExprNode(std::unique_ptr<ASTNode> expr)
        : expression(std::move(expr)) {}

    void accept(Visitor&) override;
};

struct PostfixExprNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::string op;

    PostfixExprNode(std::unique_ptr<ASTNode> expr, const std::string& op)
        : expr(std::move(expr)), op(op) {}

    void accept(Visitor&) override;
};
    
