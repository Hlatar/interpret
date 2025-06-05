#pragma once

#include <string>
#include <vector>
#include <memory>


struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(class Visitor&) = 0;
};


struct DeclNode : ASTNode {
};

// Промежуточный класс для операторов
struct StmtNode : ASTNode {
};

// Промежуточный класс для выражений
struct ExprNode : ASTNode {
};

// Декларации
struct TranslationUnitNode : DeclNode {
    std::vector<std::unique_ptr<ASTNode>> declarations;
    TranslationUnitNode(std::vector<std::unique_ptr<ASTNode>> decls) : declarations(std::move(decls)) {}
    void accept(Visitor&) override;
};

struct TypeNode : DeclNode {
    std::string type_name;
    bool is_const = false;
    bool is_unsigned = false;
    TypeNode(std::string name, bool isConst = false, bool isUnsigned = false)
        : type_name(std::move(name)), is_const(isConst), is_unsigned(isUnsigned) {}
    void accept(Visitor&) override;
};

struct DeclaratorNode : DeclNode {
    std::string name;
    std::unique_ptr<ASTNode> array_size;
    DeclaratorNode(const std::string& name, std::unique_ptr<ASTNode> size = nullptr)
        : name(name), array_size(std::move(size)) {}
    void accept(Visitor&) override;
};

struct InitDeclaratorNode : DeclNode {
    std::unique_ptr<DeclaratorNode> declarator;
    std::unique_ptr<ASTNode> initializer;
    InitDeclaratorNode(std::unique_ptr<DeclaratorNode> decl, std::unique_ptr<ASTNode> init = nullptr)
        : declarator(std::move(decl)), initializer(std::move(init)) {}
    void accept(Visitor&) override;
};

struct InitListNode : DeclNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
    InitListNode(std::vector<std::unique_ptr<ASTNode>> elements) : elements(std::move(elements)) {}
    void accept(Visitor&) override;
};

struct VarDeclNode : DeclNode {
    std::unique_ptr<ASTNode> type;
    std::vector<std::unique_ptr<InitDeclaratorNode>> declarators;
    bool is_const;
    VarDeclNode(std::unique_ptr<ASTNode> type, std::vector<std::unique_ptr<InitDeclaratorNode>> decls, bool is_const = false)
        : type(std::move(type)), declarators(std::move(decls)), is_const(is_const) {}
    void accept(Visitor&) override;
};

struct ParamDeclNode : DeclNode {
    std::unique_ptr<ASTNode> type;
    std::unique_ptr<DeclaratorNode> declarator;
    bool is_const;
    ParamDeclNode(std::unique_ptr<ASTNode> type, std::unique_ptr<DeclaratorNode> declarator, bool is_const = false)
        : type(std::move(type)), declarator(std::move(declarator)), is_const(is_const) {}
    void accept(Visitor&) override;
};

struct FuncDeclNode : DeclNode {
    std::unique_ptr<ASTNode> return_type;
    std::string name;
    std::vector<std::unique_ptr<ParamDeclNode>> params;
    std::unique_ptr<ASTNode> body;
    bool is_const;
    FuncDeclNode(std::unique_ptr<ASTNode> ret, const std::string& name,
                 std::vector<std::unique_ptr<ParamDeclNode>> params,
                 std::unique_ptr<ASTNode> body = nullptr, bool is_const = false)
        : return_type(std::move(ret)), name(name), params(std::move(params)), body(std::move(body)), is_const(is_const) {}
    void accept(Visitor&) override;
};

struct StructDeclNode : DeclNode {
    std::string name;
    std::vector<std::unique_ptr<VarDeclNode>> members;
    StructDeclNode(const std::string& name, std::vector<std::unique_ptr<VarDeclNode>> members)
        : name(name), members(std::move(members)) {}
    void accept(Visitor&) override;
};

struct NamespaceDeclNode : DeclNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> declarations;
    NamespaceDeclNode(std::string name, std::vector<std::unique_ptr<ASTNode>> declarations)
        : name(std::move(name)), declarations(std::move(declarations)) {}
    void accept(Visitor&) override;
};

// Операторы
struct BlockStatementNode : StmtNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockStatementNode(std::vector<std::unique_ptr<ASTNode>> stmts) : statements(std::move(stmts)) {}
    void accept(Visitor&) override;
};

struct IfStatementNode : StmtNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;
    IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then_b, std::unique_ptr<ASTNode> else_b = nullptr)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
    void accept(Visitor&) override;
};

struct WhileLoopNode : StmtNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    WhileLoopNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body)
        : condition(std::move(cond)), body(std::move(body)) {}
    void accept(Visitor&) override;
};

struct DoWhileLoopNode : StmtNode {
    std::unique_ptr<ASTNode> body;
    std::unique_ptr<ASTNode> condition;
    DoWhileLoopNode(std::unique_ptr<ASTNode> body, std::unique_ptr<ASTNode> cond)
        : body(std::move(body)), condition(std::move(cond)) {}
    void accept(Visitor&) override;
};

struct ForLoopNode : StmtNode {
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    ForLoopNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond,
                std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> body)
        : init(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(body)) {}
    void accept(Visitor&) override;
};

struct ReturnStatementNode : StmtNode {
    std::unique_ptr<ASTNode> expression;
    ReturnStatementNode(std::unique_ptr<ASTNode> expr = nullptr) : expression(std::move(expr)) {}
    void accept(Visitor&) override;
};

struct BreakStmtNode : StmtNode {
    BreakStmtNode() = default;
    void accept(Visitor&) override;
};

struct ContinueStmtNode : StmtNode {
    ContinueStmtNode() = default;
    void accept(Visitor&) override;
};

struct ReadStmtNode : StmtNode {
    std::unique_ptr<ASTNode> argument;
    ReadStmtNode(std::unique_ptr<ASTNode> arg) : argument(std::move(arg)) {}
    void accept(Visitor&) override;
};

struct PrintStmtNode : StmtNode {
    std::unique_ptr<ASTNode> argument;
    PrintStmtNode(std::unique_ptr<ASTNode> arg) : argument(std::move(arg)) {}
    void accept(Visitor&) override;
};

struct StaticAssertNode : StmtNode {
    std::unique_ptr<ASTNode> condition;
    std::string message;
    StaticAssertNode(std::unique_ptr<ASTNode> cond, std::string msg)
        : condition(std::move(cond)), message(std::move(msg)) {}
    void accept(Visitor&) override;
};

// Выражения
struct BinaryExprNode : ExprNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryExprNode(const std::string& op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
    void accept(Visitor&) override;
};

struct UnaryExprNode : ExprNode {
    std::string op;
    std::unique_ptr<ASTNode> operand;
    UnaryExprNode(const std::string& op, std::unique_ptr<ASTNode> operand)
        : op(op), operand(std::move(operand)) {}
    void accept(Visitor&) override;
};

struct TernaryExprNode : ExprNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_expr;
    std::unique_ptr<ASTNode> else_expr;
    TernaryExprNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then_e, std::unique_ptr<ASTNode> else_e)
        : condition(std::move(cond)), then_expr(std::move(then_e)), else_expr(std::move(else_e)) {}
    void accept(Visitor&) override;
};

struct CastExprNode : ExprNode {
    std::unique_ptr<ASTNode> type;
    std::unique_ptr<ASTNode> expression;
    CastExprNode(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> expr)
        : type(std::move(type)), expression(std::move(expr)) {}
    void accept(Visitor&) override;
};

struct SubscriptExprNode : ExprNode {
    std::unique_ptr<ASTNode> array;
    std::unique_ptr<ASTNode> index;
    SubscriptExprNode(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> index)
        : array(std::move(array)), index(std::move(index)) {}
    void accept(Visitor&) override;
};

struct CallExprNode : ExprNode {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    CallExprNode(std::unique_ptr<ASTNode> callee, std::vector<std::unique_ptr<ASTNode>> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}
    void accept(Visitor&) override;
};

struct LiteralExprNode : ExprNode {
    std::unique_ptr<ASTNode> type;
    std::string value;
    LiteralExprNode(std::unique_ptr<ASTNode> type, const std::string& value)
        : type(std::move(type)), value(value) {}
    void accept(Visitor&) override;
};

struct IdentifierExprNode : ExprNode {
    std::string name;
    IdentifierExprNode(const std::string& name) : name(name) {}
    void accept(Visitor&) override;
};

struct GroupExprNode : ExprNode {
    std::unique_ptr<ASTNode> expression;
    GroupExprNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}
    void accept(Visitor&) override;
};

struct PostfixExprNode : ExprNode {
    std::unique_ptr<ASTNode> expr;
    std::string op;
    PostfixExprNode(std::unique_ptr<ASTNode> expr, const std::string& op)
        : expr(std::move(expr)), op(op) {}
    void accept(Visitor&) override;
};

struct ScopedIdentifierExprNode : ExprNode {
    std::vector<std::string> path;
    ScopedIdentifierExprNode(std::vector<std::string> path) : path(std::move(path)) {}
    void accept(Visitor&) override;
};

struct AssignmentExprNode : ExprNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op;
    AssignmentExprNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, const std::string& op = "=")
        : left(std::move(left)), right(std::move(right)), op(op) {}
    void accept(Visitor&) override;
};

struct MemberAccessExprNode : ExprNode {
    std::unique_ptr<ASTNode> object;
    std::string member;
    std::string op;
    MemberAccessExprNode(std::unique_ptr<ASTNode> object, const std::string& member, const std::string& op)
        : object(std::move(object)), member(member), op(op) {}
    void accept(Visitor&) override;
};

struct SizeofExprNode : ExprNode {
    std::unique_ptr<ASTNode> operand;
    bool isType;
    SizeofExprNode(std::unique_ptr<ASTNode> op, bool isType) : operand(std::move(op)), isType(isType) {}
    void accept(Visitor&) override;
};

struct ExitExprNode : ExprNode {
    std::vector<std::unique_ptr<ASTNode>> arguments;
    ExitExprNode(std::vector<std::unique_ptr<ASTNode>> args) : arguments(std::move(args)) {}
    void accept(Visitor&) override;
};

struct AssertExprNode : ExprNode {
    std::vector<std::unique_ptr<ASTNode>> arguments;
    AssertExprNode(std::vector<std::unique_ptr<ASTNode>> args) : arguments(std::move(args)) {}
    void accept(Visitor&) override;
};


