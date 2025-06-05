#include "../inc/sema.hpp"

void SemanticAnalyzer::analyze(ASTNode& root) {
    if (auto* tu = dynamic_cast<TranslationUnitNode*>(&root)) {
        collectFunctionSignatures(*tu);  // Сначала сигнатуры
    }
    root.accept(*this);  // Потом всё остальное
}

void SemanticAnalyzer::visit(TranslationUnitNode& node){
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
}

void SemanticAnalyzer::visit(VarDeclNode& node) {
    TypeNode* typeNode = dynamic_cast<TypeNode*>(node.type.get());
    if (!typeNode) {
        throw SemanticError("Invalid type in variable declaration");
    }

    std::shared_ptr<Type> type;

    // Пытаемся найти в таблице типов
    auto structIt = typeTable.find(typeNode->type_name);
    if (structIt != typeTable.end()) {
        type = structIt->second;
    } else {
        // Встроенный тип
        type = std::make_shared<BuiltinType>(typeNode->type_name, typeNode->is_const, typeNode->is_unsigned);
    }

    for (auto& decl : node.declarators) {
        if (!declareVariable(decl->declarator->name, type)) {
            throw SemanticError("Redefinition of variable: " + decl->declarator->name);
        }
        // После добавления в таблицу — проверка инициализации
        if (decl->initializer) {
            decl->accept(*this);
        }
    }
}

void SemanticAnalyzer::collectFunctionSignatures(TranslationUnitNode& node) {
    for (auto& decl : node.declarations) {
        if (auto func = dynamic_cast<FuncDeclNode*>(decl.get())) {
            FunctionSignature sig;
            sig.name = func->name;

            TypeNode* returnTypeNode = dynamic_cast<TypeNode*>(func->return_type.get());
            if (!returnTypeNode) {
                throw SemanticError("Invalid return type for function " + sig.name);
            }

            sig.returnType = std::make_shared<BuiltinType>(returnTypeNode->type_name);

            for (auto& param : func->params) {
                TypeNode* paramTypeNode = dynamic_cast<TypeNode*>(param->type.get());
                if (!paramTypeNode) {
                    throw SemanticError("Invalid parameter type in function " + sig.name);
                }
                sig.paramTypes.push_back(std::make_shared<BuiltinType>(paramTypeNode->type_name));
            }

            if (functionTable.count(sig.name)) {
                throw SemanticError("Function " + sig.name + " already declared");
            }

            functionTable[sig.name] = sig;
        }
    }
}






void SemanticAnalyzer::visit(FuncDeclNode& node) {
    auto it = functionTable.find(node.name);
    if (it == functionTable.end()) {
        throw SemanticError("Function not declared before body: " + node.name);
    }

    std::shared_ptr<Type> returnType = it->second.returnType;
    expectedReturnTypes.push_back(returnType);

    enterScope();
    for (size_t i = 0; i < node.params.size(); ++i) {
        auto& param = node.params[i];
        auto paramType = it->second.paramTypes[i];
        if (!declareVariable(param->declarator->name, paramType)) {
            throw SemanticError("Redefinition of parameter: " + param->declarator->name);
        }
    }

    if (node.body) {
        node.body->accept(*this);
    }

    exitScope();
    expectedReturnTypes.pop_back();
}


void SemanticAnalyzer::visit(TypeNode& node) {
    // Заглушка: пока просто игнорируем или выбрасываем ошибку
    
}

void SemanticAnalyzer::visit(DeclaratorNode& node) {
    throw SemanticError("DeclaratorNode analysis not implemented yet");
}

void SemanticAnalyzer::visit(InitDeclaratorNode& node) {
    if (node.initializer) {
        node.initializer->accept(*this);

        auto initExpr = dynamic_cast<ExprNode*>(node.initializer.get());
        if (!initExpr) throw SemanticError("Invalid initializer");

        auto varType = lookupVariable(node.declarator->name);
        if (!varType) throw SemanticError("Variable not declared before initializer");

        auto initType = getType(*initExpr);
        if (!varType || !initType->equals(*varType.value())) {
            throw SemanticError("Initializer type mismatch for variable " + node.declarator->name);
        }

    }
}

void SemanticAnalyzer::visit(ParamDeclNode& node) {
    node.declarator->accept(*this);
}

void SemanticAnalyzer::visit(StructDeclNode& node) {
    auto structType = std::make_shared<StructType>(node.name);

    for (const auto& varDecl : node.members) {
        TypeNode* typeNode = dynamic_cast<TypeNode*>(varDecl->type.get());
        if (!typeNode) {
            throw SemanticError("Invalid type for member in struct " + node.name);
        }

        auto memberType = std::make_shared<BuiltinType>(typeNode->type_name);

        for (const auto& initDecl : varDecl->declarators) {
            const std::string& fieldName = initDecl->declarator->name;
            if (structType->fields.count(fieldName)) {
                throw SemanticError("Duplicate member '" + fieldName + "' in struct " + node.name);
            }
            structType->addField(fieldName, memberType);
        }
    }

    // Зарегистрируем struct как тип (в переменной/типовой таблице)
    if (typeTable.count(node.name)) {
        throw SemanticError("Redefinition of struct: " + node.name);
    }
    typeTable[node.name] = structType;

}

void SemanticAnalyzer::visit(BlockStatementNode& node) {
     enterScope();  // ← вот это критично
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    exitScope(); 
}

void SemanticAnalyzer::visit(IfStatementNode& node) {
    auto condExpr = dynamic_cast<ExprNode*>(node.condition.get());
    if (!condExpr) throw SemanticError("Invalid condition in if");

    auto condType = getType(*condExpr);
    auto builtin = std::dynamic_pointer_cast<BuiltinType>(condType);
    if (!builtin || (builtin->name != "int" && builtin->name != "bool")) {
        throw SemanticError("Condition in if-statement must be of type int or bool");
    }

    node.then_branch->accept(*this);
    if (node.else_branch) {
        node.else_branch->accept(*this);
    }
}

void SemanticAnalyzer::visit(WhileLoopNode& node) {
    enterScope();
    auto condExpr = dynamic_cast<ExprNode*>(node.condition.get());
    if (!condExpr) throw SemanticError("Invalid condition in while");

    auto condType = getType(*condExpr);
    auto builtin = std::dynamic_pointer_cast<BuiltinType>(condType);
    if (!builtin || (builtin->name != "int" && builtin->name != "bool")) {
        throw SemanticError("Condition in while-loop must be of type int or bool");
    }

    node.body->accept(*this);
    exitScope();
}

void SemanticAnalyzer::visit(DoWhileLoopNode& node) {
    node.body->accept(*this);

    auto condExpr = dynamic_cast<ExprNode*>(node.condition.get());
    if (!condExpr) throw SemanticError("Invalid condition in do-while");

    auto condType = getType(*condExpr);
    auto builtin = std::dynamic_pointer_cast<BuiltinType>(condType);
    if (!builtin || (builtin->name != "int" && builtin->name != "bool")) {
        throw SemanticError("Condition in do-while-loop must be of type int or bool");
    }
}

void SemanticAnalyzer::visit(ForLoopNode& node) {
    enterScope();
    if (node.init) node.init->accept(*this);

    if (node.condition) {
        auto condExpr = dynamic_cast<ExprNode*>(node.condition.get());
        if (!condExpr) throw SemanticError("Invalid condition in for");

        auto condType = getType(*condExpr);
        auto builtin = std::dynamic_pointer_cast<BuiltinType>(condType);
        if (!builtin || (builtin->name != "int" && builtin->name != "bool")) {
            throw SemanticError("Condition in for-loop must be of type int or bool");
        }
    }

    if (node.increment) node.increment->accept(*this);
    node.body->accept(*this);
    exitScope();
}

void SemanticAnalyzer::visit(ReturnStatementNode& node) {
    if (!expectedReturnTypes.empty() && node.expression) {
        auto expr = dynamic_cast<ExprNode*>(node.expression.get());
        if (!expr) throw SemanticError("Invalid return expression");

        auto returnType = getType(*expr);
        if (!returnType->equals(*expectedReturnTypes.back())) {
            throw SemanticError("Return type mismatch");
        }
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
    auto left = dynamic_cast<ExprNode*>(node.left.get());
    auto right = dynamic_cast<ExprNode*>(node.right.get());
    if (!left || !right) throw SemanticError("Invalid binary expression");

    auto leftType = getType(*left);
    auto rightType = getType(*right);
    if (!leftType->equals(*rightType)) {
        throw SemanticError("Type mismatch in binary expression");
    }
}

void SemanticAnalyzer::visit(UnaryExprNode& node) {
    node.operand->accept(*this);
}

void SemanticAnalyzer::visit(TernaryExprNode& node) {
    auto thenExpr = dynamic_cast<ExprNode*>(node.then_expr.get());
    auto elseExpr = dynamic_cast<ExprNode*>(node.else_expr.get());
    if (!thenExpr || !elseExpr) throw SemanticError("Invalid ternary expression");

    auto thenType = getType(*thenExpr);
    auto elseType = getType(*elseExpr);
    if (!thenType->equals(*elseType)) {
        throw SemanticError("Ternary branches have different types");
    }
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
    auto lhs = dynamic_cast<ExprNode*>(node.left.get());
    auto rhs = dynamic_cast<ExprNode*>(node.right.get());
    if (!lhs || !rhs) throw SemanticError("Invalid assignment");

    auto lhsType = getType(*lhs);
    auto rhsType = getType(*rhs);
    if (!lhsType->equals(*rhsType)) {
        throw SemanticError("Type mismatch in assignment");
    }
}

void SemanticAnalyzer::visit(MemberAccessExprNode& node) {
    auto baseExpr = dynamic_cast<ExprNode*>(node.object.get());
    if (!baseExpr) throw SemanticError("Invalid base in member access");

    auto baseType = getType(*baseExpr);

    auto structType = std::dynamic_pointer_cast<StructType>(baseType);
    if (!structType) {
        throw SemanticError("Member access on non-struct type");
    }

    if (!structType->getFieldType(node.member)) {
        throw SemanticError("Struct '" + structType->name + "' has no member '" + node.member + "'");
    }
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

std::shared_ptr<Type> SemanticAnalyzer::getType(ASTNode& expr) {
    auto exprNode = dynamic_cast<ExprNode*>(&expr);
    if (!exprNode) {
        throw SemanticError("Node is not an expression");
    }
    expr.accept(*this); // пройти поддерево, если нужно

    if (auto id = dynamic_cast<IdentifierExprNode*>(&expr)) {
        auto varType = lookupVariable(id->name);
        if (!varType) {
            throw SemanticError("Undeclared identifier: " + id->name);
        }
        return *varType;
    }

    if (auto lit = dynamic_cast<LiteralExprNode*>(&expr)) {
        if (!lit->type) {
            throw SemanticError("Literal has no type");
        }
        TypeNode* tnode = dynamic_cast<TypeNode*>(lit->type.get());
        if (!tnode) {
            throw SemanticError("Literal type invalid");
        }
        return std::make_shared<BuiltinType>(tnode->type_name, tnode->is_const, tnode->is_unsigned);
    }

    if (auto bin = dynamic_cast<BinaryExprNode*>(&expr)) {
        auto leftType = getType(*bin->left);
        auto rightType = getType(*bin->right);
        if (!leftType->equals(*rightType)) {
            throw SemanticError("Binary expression operands must have same type");
        }
        return leftType;
    }

    if (auto un = dynamic_cast<UnaryExprNode*>(&expr)) {
        return getType(*un->operand);
    }

    if (auto tern = dynamic_cast<TernaryExprNode*>(&expr)) {
        auto thenType = getType(*tern->then_expr);
        auto elseType = getType(*tern->else_expr);
        if (!thenType->equals(*elseType)) {
            throw SemanticError("Ternary branches have different types");
        }
        return thenType;
    }

    if (auto cast = dynamic_cast<CastExprNode*>(&expr)) {
        TypeNode* tnode = dynamic_cast<TypeNode*>(cast->type.get());
        if (!tnode) {
            throw SemanticError("Invalid cast type");
        }
        return std::make_shared<BuiltinType>(tnode->type_name, tnode->is_const, tnode->is_unsigned);
    }

    if (auto assign = dynamic_cast<AssignmentExprNode*>(&expr)) {
        auto lhsType = getType(*assign->left);
        auto rhsType = getType(*assign->right);
        if (!lhsType->equals(*rhsType)) {
            throw SemanticError("Type mismatch in assignment");
        }
        return lhsType;
    }

    if (auto call = dynamic_cast<CallExprNode*>(&expr)) {
        auto callee = dynamic_cast<IdentifierExprNode*>(call->callee.get());
        if (!callee) {
            throw SemanticError("Only simple function calls are supported");
        }

        auto it = functionTable.find(callee->name);
        if (it == functionTable.end()) {
            throw SemanticError("Call to undeclared function: " + callee->name);
        }

        const auto& sig = it->second;
        if (sig.paramTypes.size() != call->arguments.size()) {
            throw SemanticError("Incorrect number of arguments in call to " + sig.name);
        }

        for (size_t i = 0; i < call->arguments.size(); ++i) {
            auto actual = getType(*call->arguments[i]);
            if (!actual->equals(*sig.paramTypes[i])) {
                throw SemanticError("Argument " + std::to_string(i+1) + " in call to " + sig.name + " has incorrect type");
            }
        }

        return sig.returnType;
    }

    if (auto group = dynamic_cast<GroupExprNode*>(&expr)) {
        return getType(*group->expression);
    }

    if (auto post = dynamic_cast<PostfixExprNode*>(&expr)) {
        return getType(*post->expr);
    }

    if (auto subscr = dynamic_cast<SubscriptExprNode*>(&expr)) {
        return getType(*subscr->array); // TODO: проверить, что это массив
    }

    if (auto mem = dynamic_cast<MemberAccessExprNode*>(&expr)) {
        auto baseType = getType(*mem->object);
        auto structType = std::dynamic_pointer_cast<StructType>(baseType);
        if (!structType) {
            throw SemanticError("Member access on non-struct type");
        }

        auto fieldType = structType->getFieldType(mem->member);
        if (!fieldType) {
            throw SemanticError("Struct '" + structType->name + "' has no member '" + mem->member + "'");
        }

        return fieldType;
    }

    if (auto scoped = dynamic_cast<ScopedIdentifierExprNode*>(&expr)) {
        std::string name = scoped->getName();  // <-- правильно достаём имя
        auto varType = lookupVariable(name);   // пока игнорируем namespace-путь
        if (!varType) {
            throw SemanticError("Undeclared scoped identifier: " + name);
        }
        return *varType;
    }

    

    throw SemanticError("Cannot infer type of expression");
}
