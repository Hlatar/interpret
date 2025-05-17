#include "../inc/printer.hpp"
#include <iostream>

void PrintVisitor::print(ASTNode& node) {
    indent_level = 0;
    node.accept(*this);
    std::cout << std::endl;
}

void PrintVisitor::indent() const {
    for (int i = 0; i < indent_level; ++i) {
        std::cout << "  ";
    }
}

void PrintVisitor::visit(TranslationUnitNode& node) {
    indent(); std::cout << "TranslationUnitNode\n";
    ++indent_level;
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
    --indent_level;
}

void PrintVisitor::visit(TypeNode& node) {
    indent(); std::cout << "TypeNode: " << (node.is_const ? "const " : "") << node.type_name << "\n";
}

void PrintVisitor::visit(DeclaratorNode& node) {
    indent(); std::cout << "DeclaratorNode: " << node.name;
    if (node.array_size) {
        std::cout << " [";
        node.array_size->accept(*this);
        std::cout << "]";
    }
    std::cout << "\n";
}

void PrintVisitor::visit(InitDeclaratorNode& node) {
    indent(); std::cout << "InitDeclaratorNode\n";
    ++indent_level;
    node.declarator->accept(*this);
    if (node.initializer) {
        indent(); std::cout << "Initializer:\n";
        ++indent_level;
        node.initializer->accept(*this);
        --indent_level;
    }
    --indent_level;
}

void PrintVisitor::visit(VarDeclNode& node) {
    indent(); std::cout << "VarDeclNode" << (node.is_const ? " (const)" : "") << "\n";
    ++indent_level;
    node.type->accept(*this);
    for (auto& decl : node.declarators) {
        decl->accept(*this);
    }
    --indent_level;
}

void PrintVisitor::visit(ParamDeclNode& node) {
    indent(); std::cout << "ParamDeclNode" << (node.is_const ? " (const)" : "") << "\n";
    ++indent_level;
    node.type->accept(*this);
    node.declarator->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(FuncDeclNode& node) {
    indent(); std::cout << "FuncDeclNode: " << node.name << (node.is_const ? " (const)" : "") << "\n";
    ++indent_level;
    indent(); std::cout << "Return type:\n";
    ++indent_level;
    node.return_type->accept(*this);
    --indent_level;

    indent(); std::cout << "Parameters:\n";
    ++indent_level;
    for (auto& param : node.params) {
        param->accept(*this);
    }
    --indent_level;

    if (node.body) {
        indent(); std::cout << "Body:\n";
        ++indent_level;
        node.body->accept(*this);
        --indent_level;
    }
    --indent_level;
}

void PrintVisitor::visit(StructDeclNode& node) {
    indent(); std::cout << "StructDeclNode: " << node.name << "\n";
    ++indent_level;
    for (auto& member : node.members) {
        member->accept(*this);
    }
    --indent_level;
}

void PrintVisitor::visit(BlockStatementNode& node) {
    indent(); std::cout << "BlockStatementNode\n";
    ++indent_level;
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    --indent_level;
}

void PrintVisitor::visit(IfStatementNode& node) {
    indent(); std::cout << "IfStatementNode\n";
    ++indent_level;
    indent(); std::cout << "Condition:\n";
    ++indent_level;
    node.condition->accept(*this);
    --indent_level;

    indent(); std::cout << "Then:\n";
    ++indent_level;
    node.then_branch->accept(*this);
    --indent_level;

    if (node.else_branch) {
        indent(); std::cout << "Else:\n";
        ++indent_level;
        node.else_branch->accept(*this);
        --indent_level;
    }
    --indent_level;
}

void PrintVisitor::visit(WhileLoopNode& node) {
    indent(); std::cout << "WhileLoopNode\n";
    ++indent_level;
    indent(); std::cout << "Condition:\n";
    ++indent_level;
    node.condition->accept(*this);
    --indent_level;

    indent(); std::cout << "Body:\n";
    ++indent_level;
    node.body->accept(*this);
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(DoWhileLoopNode& node) {
    indent(); std::cout << "DoWhileLoopNode\n";
    ++indent_level;
    indent(); std::cout << "Body:\n";
    ++indent_level;
    node.body->accept(*this);
    --indent_level;
    indent(); std::cout << "Condition:\n";
    ++indent_level;
    node.condition->accept(*this);
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(ForLoopNode& node) {
    indent(); std::cout << "ForLoopNode\n";
    ++indent_level;
    if (node.init) {
        indent(); std::cout << "Init:\n";
        ++indent_level;
        node.init->accept(*this);
        --indent_level;
    }
    if (node.condition) {
        indent(); std::cout << "Condition:\n";
        ++indent_level;
        node.condition->accept(*this);
        --indent_level;
    }
    if (node.increment) {
        indent(); std::cout << "Increment:\n";
        ++indent_level;
        node.increment->accept(*this);
        --indent_level;
    }
    indent(); std::cout << "Body:\n";
    ++indent_level;
    node.body->accept(*this);
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(ReturnStatementNode& node) {
    indent(); std::cout << "ReturnStatementNode\n";
    if (node.expression) {
        ++indent_level;
        node.expression->accept(*this);
        --indent_level;
    }
}

void PrintVisitor::visit(BreakStatementNode&) {
    indent(); std::cout << "BreakStatementNode\n";
}

void PrintVisitor::visit(ContinueStatementNode&) {
    indent(); std::cout << "ContinueStatementNode\n";
}

void PrintVisitor::visit(BinaryExprNode& node) {
    indent(); std::cout << "BinaryExprNode: " << node.op << "\n";
    ++indent_level;
    node.left->accept(*this);
    node.right->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(UnaryExprNode& node) {
    indent(); std::cout << "UnaryExprNode: " << node.op << "\n";
    ++indent_level;
    node.operand->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(TernaryExprNode& node) {
    indent(); std::cout << "TernaryExprNode\n";
    ++indent_level;
    node.condition->accept(*this);
    node.then_expr->accept(*this);
    node.else_expr->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(CastExprNode& node) {
    indent(); std::cout << "CastExprNode\n";
    ++indent_level;
    node.type->accept(*this);
    node.expression->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(SubscriptExprNode& node) {
    indent(); std::cout << "SubscriptExprNode\n";
    ++indent_level;
    node.array->accept(*this);
    node.index->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(CallExprNode& node) {
    indent(); std::cout << "CallExprNode\n";
    ++indent_level;
    node.callee->accept(*this);
    indent(); std::cout << "Arguments:\n";
    ++indent_level;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(AccessExprNode& node) {
    indent(); std::cout << "AccessExprNode: " << (node.is_pointer_access ? "->" : ".") << node.member << "\n";
    ++indent_level;
    node.object->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(LiteralExprNode& node) {
    indent(); std::cout << "LiteralExprNode: " << node.value << "\n";
}

void PrintVisitor::visit(IdentifierExprNode& node) {
    indent(); std::cout << "IdentifierExprNode: " << node.name << "\n";
}

void PrintVisitor::visit(GroupExprNode& node) {
    indent(); std::cout << "GroupExprNode\n";
    ++indent_level;
    node.expression->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(PostfixExprNode& node){
    indent(); std::cout << "PostfixExprNode: " << node.op << "\n";
    ++indent_level;
    node.expr->accept(*this);
    --indent_level;
}

void PrintVisitor::visit(InitListNode& node) {
    indent(); std::cout << "InitListNode\n";
    ++indent_level;
    indent(); std::cout << "Elements:\n";
    ++indent_level;
    for (auto& element : node.elements) {
        element->accept(*this);
    }
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(BreakStmtNode& node) {
    indent();
    std::cout << "BreakStmtNode\n";
}

void PrintVisitor::visit(ContinueStmtNode& node) {
    indent();
    std::cout << "ContinueStmtNode\n";
}

void PrintVisitor::visit(SizeofExprNode& node) {
    indent(); std::cout << "SizeofExprNode\n";
    ++indent_level;
    indent(); std::cout << (node.isType ? "Type:\n" : "Expression:\n");
    ++indent_level;
    node.operand->accept(*this);
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(StaticAssertNode& node) {
    indent(); std::cout << "StaticAssertNode\n";
    ++indent_level;
    indent(); std::cout << "Condition:\n";
    ++indent_level;
    node.condition->accept(*this);
    --indent_level;
    if (!node.message.empty()) {
        indent(); std::cout << "Message: " << node.message << "\n";
    }
    --indent_level;
}

void PrintVisitor::visit(ExitExprNode& node) {
    indent(); std::cout << "ExitExprNode: "  << "\n";
    ++indent_level;
    indent(); std::cout << "Arguments:\n";
    ++indent_level;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
    }
    --indent_level;
    --indent_level;
}

void PrintVisitor::visit(AssertExprNode& node) {
    indent(); std::cout << "AssertExprNode: "  << "\n";
    ++indent_level;
    indent(); std::cout << "Arguments:\n";
    ++indent_level;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
    }
    --indent_level;
    --indent_level;
}