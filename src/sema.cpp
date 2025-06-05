#include "../inc/sema.hpp"

void SemanticAnalyzer::analyze(ASTNode& root){
    root.accept(*this);
}

void SemanticAnalyzer::visit(TranslationUnitNode& node){
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
}

void SemanticAnalyzer::visit(VarDeclNode& node) {
    // Проверяем, что node.type является TypeNode
    TypeNode* typeNode = dynamic_cast<TypeNode*>(node.type.get());
    if (!typeNode) {
        throw SemanticError("Invalid type in variable declaration");
    }
    // Создаём BuiltinType из type_name
    auto type = std::make_shared<BuiltinType>(typeNode->type_name, typeNode->is_const, typeNode->is_unsigned);
    for (auto& decl : node.declarators) {
        decl->accept(*this);
        declareVariable(decl->declarator->name, type);
    }
}

void SemanticAnalyzer::visit(FuncDeclNode& node) {
    FunctionSignature sig;
    sig.name = node.name;

    // Проверяем возвращаемый тип
    TypeNode* returnTypeNode = dynamic_cast<TypeNode*>(node.return_type.get());
    if (!returnTypeNode) {
        throw SemanticError("Invalid return type for function " + node.name);
    }
    sig.returnType = std::make_shared<BuiltinType>(returnTypeNode->type_name);

    // Обрабатываем параметры
    for (auto& param : node.params) {
        TypeNode* paramTypeNode = dynamic_cast<TypeNode*>(param->type.get());
        if (!paramTypeNode) {
            throw SemanticError("Invalid parameter type in function " + node.name);
        }
        sig.paramTypes.push_back(std::make_shared<BuiltinType>(paramTypeNode->type_name));
    }

    // Проверяем переопределение функции
    if (functionTable.find(node.name) != functionTable.end()) {
        throw SemanticError("Function " + node.name + " already declared");
    }
    functionTable[node.name] = sig;

    // Анализ параметров и тела функции
    enterScope();
    for (auto& param : node.params) {
        TypeNode* paramTypeNode = dynamic_cast<TypeNode*>(param->type.get());
        if (!paramTypeNode) {
            throw SemanticError("Invalid parameter type in function " + node.name);
        }
        auto paramType = std::make_shared<BuiltinType>(paramTypeNode->type_name);
        declareVariable(param->declarator->name, paramType);
    }
    if (node.body) {
        node.body->accept(*this);
    }
    exitScope();
}

void SemanticAnalyzer::visit(TypeNode& node) {
    // Заглушка: пока просто игнорируем или выбрасываем ошибку
    throw SemanticError("TypeNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(DeclaratorNode& node) {
    throw SemanticError("DeclaratorNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(InitDeclaratorNode& node) {
    throw SemanticError("InitDeclaratorNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(ParamDeclNode& node) {
    throw SemanticError("ParamDeclNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(StructDeclNode& node) {
    throw SemanticError("StructDeclNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(BlockStatementNode& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(IfStatementNode& node) {
    node.condition->accept(*this);
    node.then_branch->accept(*this);
    if (node.else_branch) {
        node.else_branch->accept(*this);
    }
}

void SemanticAnalyzer::visit(WhileLoopNode& node) {
    node.condition->accept(*this);
    node.body->accept(*this);
}

void SemanticAnalyzer::visit(DoWhileLoopNode& node) {
    node.body->accept(*this);
    node.condition->accept(*this);
}

void SemanticAnalyzer::visit(ForLoopNode& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.increment) node.increment->accept(*this);
    node.body->accept(*this);
}

void SemanticAnalyzer::visit(ReturnStatementNode& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void SemanticAnalyzer::visit(BreakStmtNode& node) {
    // Проверка, что break находится внутри цикла, может быть добавлена позже
}

void SemanticAnalyzer::visit(ContinueStmtNode& node) {
    // Проверка, что continue находится внутри цикла, может быть добавлена позже
}

void SemanticAnalyzer::visit(ReadStmtNode& node) {
    if (node.argument) {
        node.argument->accept(*this);
    }
}

void SemanticAnalyzer::visit(PrintStmtNode& node) {
    if (node.argument) {
        node.argument->accept(*this);
    }
}

void SemanticAnalyzer::visit(StaticAssertNode& node) {
    node.condition->accept(*this);
}

void SemanticAnalyzer::visit(BinaryExprNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}

void SemanticAnalyzer::visit(UnaryExprNode& node) {
    node.operand->accept(*this);
}

void SemanticAnalyzer::visit(TernaryExprNode& node) {
    node.condition->accept(*this);
    node.then_expr->accept(*this);
    node.else_expr->accept(*this);
}

void SemanticAnalyzer::visit(CastExprNode& node) {
    node.type->accept(*this);
    node.expression->accept(*this);
}

void SemanticAnalyzer::visit(SubscriptExprNode& node) {
    node.array->accept(*this);
    node.index->accept(*this);
}

void SemanticAnalyzer::visit(CallExprNode& node) {
    node.callee->accept(*this);
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(LiteralExprNode& node) {
    if (node.type) {
        node.type->accept(*this);
    }
}

void SemanticAnalyzer::visit(IdentifierExprNode& node) {
    // Проверка, существует ли переменная, может быть добавлена позже
}

void SemanticAnalyzer::visit(GroupExprNode& node) {
    node.expression->accept(*this);
}

void SemanticAnalyzer::visit(PostfixExprNode& node) {
    node.expr->accept(*this);
}

void SemanticAnalyzer::visit(InitListNode& node) {
    for (auto& elem : node.elements) {
        elem->accept(*this);
    }
}

void SemanticAnalyzer::visit(SizeofExprNode& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }
}

void SemanticAnalyzer::visit(ExitExprNode& node) {
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(AssertExprNode& node) {
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(AssignmentExprNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}

void SemanticAnalyzer::visit(MemberAccessExprNode& node) {
    node.object->accept(*this);
}

void SemanticAnalyzer::visit(NamespaceDeclNode& node) {
    enterScope();
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
    exitScope();
}

void SemanticAnalyzer::visit(ScopedIdentifierExprNode& node) {
    // Проверка области видимости может быть добавлена позже
}