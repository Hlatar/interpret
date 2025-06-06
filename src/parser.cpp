#include "../inc/parser.hpp"
#include "iostream"
#include "../inc/token.hpp"
#include <memory>
#include <unordered_set>

std::unordered_set<std::string> knownTypes = {
    "int", "double", "char", "void", "bool", "short", "long", "float"
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ASTNode> Parser::parse() {
    return parseTranslationUnit();
}

std::unique_ptr<ASTNode> Parser::parseTranslationUnit(){
    std::vector<std::unique_ptr<ASTNode>> decls;
    while (!isAtEnd()) {
        if(check(TokenType::COMMENT_STR)){
            advance();
            continue;
        } 
        auto node = parseDeclaration();  // <- вернёт конкретный подтип ASTNode
        if (node) decls.push_back(std::move(node));
    }
    return std::make_unique<TranslationUnitNode>(std::move(decls));
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (match(TokenType::NAMESPACE)){
        return parseNamespaceDeclaration();
    }
    if (check(TokenType::STRUCT)){
        return parseStructDeclaration();
    }
    if (isType()) {
        size_t saved_pos = current; // Сохраняем текущую позицию
        auto type = parseType(); // Пробуем разобрать тип
        if (check(TokenType::ID) && peekNext().type == TokenType::LBRACE) {
            current = saved_pos; // Восстанавливаем позицию для функции
            return parseFuncDeclaration();
        } else {
            current = saved_pos; // Восстанавливаем позицию для переменной
            return parseVarDeclaration();
        }
    }
    reportError("Unknown declaration");
    return nullptr;
}

std::unique_ptr<VarDeclNode> Parser::parseVarDeclaration(){
    auto type = parseType();
    std::vector<std::unique_ptr<InitDeclaratorNode>> declarators;
    do {
        auto declarator = parseDeclarator();
        std::unique_ptr<ASTNode> initializer = nullptr;
        if (match(TokenType::EQUAL)) {
            if (match(TokenType::LFIGUREBRACE)) {
                std::vector<std::unique_ptr<ASTNode>> initList;
                if (!check(TokenType::RFIGUREBRACE)) {
                    do {
                        initList.push_back(parseExpression());
                    } while (match(TokenType::COMMA));
                }
                expect(TokenType::RFIGUREBRACE, "Expected '}'");
                initializer = std::make_unique<InitListNode>(std::move(initList));
            } else {
            initializer = parseExpression();
            }
        }
        declarators.push_back(std::make_unique<InitDeclaratorNode>(
            std::move(declarator), std::move(initializer)
        ));
    } while (match(TokenType::COMMA));
    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::make_unique<VarDeclNode>(std::move(type), std::move(declarators));
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    auto expr = parseTernary();
    if (match({TokenType::EQUAL, TokenType::PLUS_ASSIGN, TokenType::MINUS_ASSIGN,
               TokenType::MULT_ASSIGN, TokenType::DIV_ASSIGN, TokenType::MOD_ASSIGN})) {
        Token op = prev(); 
        auto value = parseAssignment(); // Поддержка цепочек, например x = y = z
        return std::make_unique<AssignmentExprNode>(std::move(expr), std::move(value), op.value);
    }
    return expr;
}


// Тернарный оператор: expr ? expr : expr
std::unique_ptr<ASTNode> Parser::parseTernary() {
    auto condition = parseBinary(); // Сначала разбираем обычное бинарное выражение
    if (match(TokenType::WHY_SIGN)) {
        auto thenExpr = parseExpression();  // Разбираем часть "then"
        expect(TokenType::DOTDOT, "Expected ':' in ternary expression.");
        auto elseExpr = parseExpression();  // Разбираем часть "else"
        return std::make_unique<TernaryExprNode>(std::move(condition), std::move(thenExpr), std::move(elseExpr));
    }
    return condition;
}

std::unique_ptr<ASTNode> Parser::parseBinary() {
    return parseLogicalOr();
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto lhs = parseLogicalAnd();

    while (match(TokenType::LOGICAL_OR)) {  // TokenType::OR должен быть для "||"
        Token op = prev();
        auto rhs = parseLogicalAnd();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto lhs = parseEquality();
    while (match(TokenType::LOGICAL_AND)) {  // TokenType::AND должен быть для "&&"
        Token op = prev();
        auto rhs = parseEquality();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    auto lhs = parseComparison();
    while (match({TokenType::LOGICAL_EQUAL, TokenType::NOT_EQUAL})) {  // ==, !=
        Token op = prev();
        auto rhs = parseComparison();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto lhs = parseAdditive();
    while (match({TokenType::LTRIBRACE, TokenType::LESS_EQUAL,
                  TokenType::RTRIBRACE, TokenType::GREATER_EQUAL})) {  // <, <=, >, >=
        Token op = prev();
        auto rhs = parseAdditive();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    auto lhs = parseMultiplicative();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = prev();
        auto rhs = parseMultiplicative();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    auto lhs = parseUnary();
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        Token op = prev();
        auto rhs = parseUnary();
        lhs = std::make_unique<BinaryExprNode>(op.value, std::move(lhs), std::move(rhs));
    }
    return lhs;
}


// Унарные выражения (например, -a, !a, ++a, --a)
std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (match({TokenType::MINUS, TokenType::SCREAMER, TokenType::INCREMENT, TokenType::DECREMENT, TokenType::AMPERSAND})) {
        Token op = prev();
        auto operand = parseUnary();  // Разбираем унарное выражение
        return std::make_unique<UnaryExprNode>(op.value, std::move(operand));
    }
    if (match(TokenType::SIZEOF)) {
        expect(TokenType::LBRACE, "Expected '(' after 'sizeof'");
        if (isType()) {
            auto type = parseType();
            expect(TokenType::RBRACE, "Expected ')' after type");
            return std::make_unique<SizeofExprNode>(std::move(type), true);
        } else {
            auto expr = parseExpression();
            expect(TokenType::RBRACE, "Expected ')' after expression");
            return std::make_unique<SizeofExprNode>(std::move(expr), false);
        }
    }
    if (match(TokenType::LBRACE)) {
        if (isType()) {
            auto type = parseType();
            expect(TokenType::RBRACE, "Expected ')' after cast type");
            auto expr = parseUnary(); // Парсим выражение с приоритетом унарных операторов
            return std::make_unique<CastExprNode>(std::move(type), std::move(expr));
        } else {
            // Откат, если не тип — это групповое выражение (expr)
            current--;
            return parsePostfix();
        }
    }
    return parsePostfix();  // Если нет унарного оператора, переходим к постфиксным выражениям
}

// Обработка постфиксных выражений (например, a++, a[i], foo())
std::unique_ptr<ASTNode> Parser::parsePostfix() {
    auto expr = parsePrimary();  // Сначала разбираем основной элемент (литералы, идентификаторы, скобки)
    while (true) {
        if (match(TokenType::LBRACE)) {       // Вызов функции
            std::vector<std::unique_ptr<ASTNode>> args;
            if (!check(TokenType::RBRACE)) {
                do {
                    args.push_back(parseExpression());  // Разбираем аргументы
                } while (match(TokenType::COMMA));
            }
            expect(TokenType::RBRACE, "Expected ')' after function arguments.");
            expr = std::make_unique<CallExprNode>(std::move(expr), std::move(args));  // Создаем узел для вызова функции
        } else if (match(TokenType::LSQUAREBRACE)) { // Индексация массива
            if (check(TokenType::RSQUAREBRACE)) {
                reportError("Expected expression inside '[]'");
            }
            auto index = parseExpression();
            expect(TokenType::RSQUAREBRACE, "Expected ']'");
            expr = std::make_unique<SubscriptExprNode>(std::move(expr), std::move(index));
        } 
        else if (match({TokenType::INCREMENT, TokenType::DECREMENT})) {  // Постфиксный инкремент/декремент
            Token op = prev();
            expr = std::make_unique<PostfixExprNode>(std::move(expr), op.value);  // Например, "++" или "--"
        }else if (match({TokenType::DOT, TokenType::ARROW})) {// Доступ к членам структур
            Token op = prev();
            expect(TokenType::ID, "Expected member name after " + op.value);
            std::string memberName = prev().value;
            expr = std::make_unique<MemberAccessExprNode>(std::move(expr), memberName, op.value);
        }else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::INT_LIT)) {
        return std::make_unique<LiteralExprNode>(
            std::make_unique<TypeNode>("int", false, false), prev().value
        );
    } else if (match(TokenType::FLOAT_LIT)) {
        return std::make_unique<LiteralExprNode>(
            std::make_unique<TypeNode>("float", false, false), prev().value
        );
    } else if (match(TokenType::CHAR_LIT)) {
        return std::make_unique<LiteralExprNode>(
            std::make_unique<TypeNode>("char", false, false), prev().value
        );
    } else if (match(TokenType::STR_LIT)) {
        return std::make_unique<LiteralExprNode>(
            std::make_unique<TypeNode>("string", false, false), prev().value
        );
    } else if (match(TokenType::ID)) {
        std::vector<std::string> path = {prev().value};
        size_t scope_count = 0;
        while (match(TokenType::SCOPE)) {
            if (++scope_count > 100) { // Защита от бесконечного цикла
                reportError("Too many scope operators (::) at token '" + peek().value + "'");
                return nullptr;
            }
            if (!match(TokenType::ID)) {
                reportError("Expected identifier after :: at token '" + peek().value + "'");
                return nullptr;
            }
            path.push_back(prev().value);
        }
        if (path.size() > 1) {
            return std::make_unique<ScopedIdentifierExprNode>(std::move(path));
        }
        return std::make_unique<IdentifierExprNode>(std::move(path[0]));
    }
    else if (match(TokenType::LBRACE)) {
        auto expr = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after expression.");
        return std::make_unique<GroupExprNode>(std::move(expr));
    }
    reportError("Expected expression.");
    return nullptr;  // обязательно, чтобы избежать warning про отсутствие return
}


std::unique_ptr<ASTNode> Parser::parseType(){
    bool isConst = false;
    bool isUnsigned = false;
    while (match(TokenType::CONST) || match(TokenType::UNSIGNED)) {// Проверяем модификаторы (можно в любом порядке)
        if (prev().type == TokenType::CONST) {
            isConst = true;
        } else if (prev().type == TokenType::UNSIGNED) {
            isUnsigned = true;
        }
    }
    if (match(TokenType::INT)) return std::make_unique<TypeNode>("int", isConst, isUnsigned);
    if (match(TokenType::DOUBLE)) return std::make_unique<TypeNode>("double", isConst, isUnsigned);
    if (match(TokenType::CHAR)) return std::make_unique<TypeNode>("char", isConst, isUnsigned);
    if (match(TokenType::VOID)) return std::make_unique<TypeNode>("void", isConst, isUnsigned);
    if (match(TokenType::BOOL)) return std::make_unique<TypeNode>("bool", isConst, isUnsigned);
    if (match(TokenType::SHORT)) return std::make_unique<TypeNode>("short", isConst, isUnsigned);
    if (match(TokenType::LONG)) return std::make_unique<TypeNode>("long", isConst, isUnsigned);
    if (match(TokenType::FLOAT)) return std::make_unique<TypeNode>("float", isConst, isUnsigned);
    if (match(TokenType::ID)) return std::make_unique<TypeNode>(prev().value, isConst, isUnsigned); // Для структур
    reportError("Expected a type");
    return nullptr;
}

std::unique_ptr<DeclaratorNode> Parser::parseDeclarator() {
    expect(TokenType::ID, "Expected identifier");
    std::string name = prev().value;
    std::unique_ptr<ASTNode> arraySize = nullptr;
    if (match(TokenType::LSQUAREBRACE)) {
        if (!check(TokenType::RSQUAREBRACE)) {
            arraySize = parseExpression();
        }
        expect(TokenType::RSQUAREBRACE, "Expected ']'");
    }
    return std::make_unique<DeclaratorNode>(name, std::move(arraySize));
}

std::unique_ptr<FuncDeclNode> Parser::parseFuncDeclaration() {
    std::unique_ptr<ASTNode> returnType = nullptr;// 1. Тип или идентификатор (для возвращаемого типа)
    if (isType()) {
        returnType = parseType();
    } else {
        reportError("Expected return type in function declaration");
        synchronize();
        return nullptr;
    }
    expect(TokenType::ID, "Expected function name");  // 2. Идентификатор функции
    std::string funcName = prev().value;// 3. Открывающая скобка (
    expect(TokenType::LBRACE, "Expected '(' after function name");// 4. Параметры
    std::vector<std::unique_ptr<ParamDeclNode>> parameters;
    if (!check(TokenType::RBRACE)) {
        do {
            auto param = parseParamDeclaration();
            if (param) {
                parameters.push_back(std::move(param));
            } else {
                reportError("Invalid parameter in function declaration");
                synchronize();
                break;
            }
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::RBRACE, "Expected ')' after parameters");// 5. Закрывающая скобка )
    std::unique_ptr<ASTNode> body = nullptr;// 6. Тело или точка с запятой
    if (match(TokenType::SEMICOLON)) {// Ничего, это прототип
    } else if (check(TokenType::LFIGUREBRACE)) {
        body = parseBlockStatement();
    } else {
        reportError("Expected ';' or function body after declaration");
        synchronize();
    }
    return std::make_unique<FuncDeclNode>(std::move(returnType), funcName, std::move(parameters), std::move(body));
}


std::unique_ptr<ParamDeclNode> Parser::parseParamDeclaration() {
    std::unique_ptr<ASTNode> type = nullptr;
    if (isType()) {
        type = parseType();
    } else {
        reportError("Expected type in parameter");
        synchronize();
        return nullptr;
    }

    auto declarator = parseDeclarator();
    if (!declarator) {
        reportError("Expected declarator after type in parameter");
        synchronize();
        return nullptr;
    }

    return std::make_unique<ParamDeclNode>(std::move(type), std::move(declarator));
}

std::unique_ptr<ASTNode> Parser::parseBlockStatement() {
    expect(TokenType::LFIGUREBRACE, "Expected '{' at beginning of block");

    std::vector<std::unique_ptr<ASTNode>> statements;

    while (!check(TokenType::RFIGUREBRACE) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        } else {
            synchronize();  // Скипаем до следующей точки синхронизации при ошибке
        }
    }

    expect(TokenType::RFIGUREBRACE, "Expected '}' at end of block");

    return std::make_unique<BlockStatementNode>(std::move(statements));
}


std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (check(TokenType::LFIGUREBRACE)) {
        return parseBlockStatement();
    }

    if (match(TokenType::IF)) {
        expect(TokenType::LBRACE, "Expected '(' after 'if'");
        auto condition = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after condition");

        auto thenBranch = parseStatement();
        std::unique_ptr<ASTNode> elseBranch = nullptr;

        if (match(TokenType::ELSE)) {
            elseBranch = parseStatement();
        }

        return std::make_unique<IfStatementNode>(
            std::move(condition), std::move(thenBranch), std::move(elseBranch)
        );
    }

    if (match(TokenType::WHILE)) {
        expect(TokenType::LBRACE, "Expected '(' after 'while'");
        auto condition = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after condition");

        auto body = parseStatement();
        return std::make_unique<WhileLoopNode>(std::move(condition), std::move(body));
    }

    if (match(TokenType::READ)) {
        expect(TokenType::LBRACE, "Expected '(' after 'read'");
        auto arg = parseExpression(); // Ожидаем &x
        expect(TokenType::RBRACE, "Expected ')' after read");
        expect(TokenType::SEMICOLON, "Expected ';' after read");
        return std::make_unique<ReadStmtNode>(std::move(arg));
    }
    if (match(TokenType::PRINT)) {
        expect(TokenType::LBRACE, "Expected '(' after 'print'");
        auto arg = parseExpression(); // Ожидаем x, 42, etc.
        expect(TokenType::RBRACE, "Expected ')' after print");
        expect(TokenType::SEMICOLON, "Expected ';' after print");
        return std::make_unique<PrintStmtNode>(std::move(arg));
    }

    if (match(TokenType::STATIC_ASSERT)) {
        expect(TokenType::LBRACE, "Expected '(' after 'static_assert'");
        auto condition = parseExpression();
        std::string message;
        if (match(TokenType::COMMA)) {
            expect(TokenType::STR_LIT, "Expected string literal for message");
            message = prev().value;
        }
        expect(TokenType::RBRACE, "Expected ')' after static_assert");
        expect(TokenType::SEMICOLON, "Expected ';' after static_assert");
        return std::make_unique<StaticAssertNode>(std::move(condition), message);
    }
    if (match(TokenType::ASSERT)) {
        expect(TokenType::LBRACE, "Expected '(' after 'assert'");
        auto condition = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after assert");
        expect(TokenType::SEMICOLON, "Expected ';' after assert");
        std::vector<std::unique_ptr<ASTNode>> args;
        args.push_back(std::move(condition));
        return std::make_unique<AssertExprNode>(std::move(args));
    }
    if (match(TokenType::EXIT)) {
        expect(TokenType::LBRACE, "Expected '(' after 'exit'");
        auto code = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after exit");
        expect(TokenType::SEMICOLON, "Expected ';' after exit");
        std::vector<std::unique_ptr<ASTNode>> args;
        args.push_back(std::move(code));
        return std::make_unique<ExitExprNode>(std::move(args));
    }

    if (match(TokenType::DO)) {
        auto body = parseStatement();
        expect(TokenType::WHILE, "Expected 'while' after do");
        expect(TokenType::LBRACE, "Expected '(' after 'while'");
        auto condition = parseExpression();
        expect(TokenType::RBRACE, "Expected ')' after condition");
        expect(TokenType::SEMICOLON, "Expected ';' after do-while");
        return std::make_unique<DoWhileLoopNode>(std::move(body), std::move(condition));
    }
    
    if (match(TokenType::BREAK)) {
    expect(TokenType::SEMICOLON, "Expected ';' after 'break'");
    return std::make_unique<BreakStmtNode>();
    } else if (match(TokenType::CONTINUE)) {
        expect(TokenType::SEMICOLON, "Expected ';' after 'continue'");
        return std::make_unique<ContinueStmtNode>();
    }

    if (match(TokenType::FOR)) {
        expect(TokenType::LBRACE, "Expected '(' after 'for'");

        std::unique_ptr<ASTNode> init = nullptr;
        std::cout << "Now in For loop parse: "<< peek().value << std::endl;
        if (!check(TokenType::SEMICOLON)) {
            if (isType()) {
                init = parseVarDeclaration(); // обрабатываем int x = 0
            } else {
                init = parseExpression(); // обрабатываем x = 0
                expect(TokenType::SEMICOLON, "Expected ';' after loop initializer");
            }
        }
        

        std::unique_ptr<ASTNode> condition = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            condition = parseExpression();
        }
        expect(TokenType::SEMICOLON, "Expected ';' after loop condition");

        std::unique_ptr<ASTNode> increment = nullptr;
        if (!check(TokenType::RBRACE)) {
            increment = parseExpression();
        }

        expect(TokenType::RBRACE, "Expected ')' after for clauses");

        auto body = parseStatement();
        return std::make_unique<ForLoopNode>(std::move(init), std::move(condition), std::move(increment), std::move(body));
    }

    if (match(TokenType::RETURN)) {
        std::unique_ptr<ASTNode> value = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            value = parseExpression();
        }
        expect(TokenType::SEMICOLON, "Expected ';' after return");
        return std::make_unique<ReturnStatementNode>(std::move(value));
    }

    // Попытка обработать объявление переменной
    if (isType()) {
        return parseVarDeclaration();
    }

    // Иначе — выражение
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::move(expr);
}


std::unique_ptr<ASTNode> Parser::parseNamespaceDeclaration() {
    expect(TokenType::ID, "Expected namespace name");
    std::string name = prev().value;
    expect(TokenType::LFIGUREBRACE, "Expected '{' after namespace name");
    std::vector<std::unique_ptr<ASTNode>> declarations;
    while (!check(TokenType::RFIGUREBRACE) && !isAtEnd()) {
        declarations.push_back(parseDeclaration());
    }
    expect(TokenType::RFIGUREBRACE, "Expected '}' after namespace body");
    return std::make_unique<NamespaceDeclNode>(name, std::move(declarations));
}

std::unique_ptr<StructDeclNode> Parser::parseStructDeclaration() {
    advance();
    if(!check(TokenType::ID)){
        reportError(" error: expected declaration Id");
    }
    std::string name = peek().value;
    std::vector<std::unique_ptr<VarDeclNode>> members;
     std::unique_ptr<ASTNode> return_type;
    
    knownTypes.insert(name);
    advance();
    if(!check(TokenType::LFIGUREBRACE)){
        reportError(" error: expected declaration '{'");
    } 
    advance();
    while(!check(TokenType::RFIGUREBRACE) && !isAtEnd()){
        members.push_back(parseVarDeclaration());
    }
    expect(TokenType::RFIGUREBRACE, "Expected '}' after struct body");
    expect(TokenType::SEMICOLON, "Expected ';' after struct declaration");
    return std::make_unique<StructDeclNode>(name, std::move(members));
}

// ===== Вспомогательные методы =====

void Parser::advance() {
    if (!isAtEnd()) current++;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::prev() const {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& error_msg) {
    if (!match(type)) reportError(error_msg);
}

void Parser::reportError(const std::string& message) {
    std::cerr << "[Parser Error] " << message << " at token '" << peek().value << "'\n";
    hadError = true;
    synchronize();
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (prev().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::INT:
            case TokenType::DOUBLE:
            case TokenType::STRUCT:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        advance();
    }
}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::END;
}

bool Parser::isType() {
    size_t lookahead = current;
    while (tokens[lookahead].type == TokenType::CONST || tokens[lookahead].type == TokenType::UNSIGNED) {
        lookahead++;
    }
    if (tokens[lookahead].type == TokenType::INT ||
        tokens[lookahead].type == TokenType::CHAR ||
        tokens[lookahead].type == TokenType::VOID ||
        tokens[lookahead].type == TokenType::DOUBLE ||
        tokens[lookahead].type == TokenType::SHORT ||
        tokens[lookahead].type == TokenType::FLOAT ||
        tokens[lookahead].type == TokenType::LONG ||
        tokens[lookahead].type == TokenType::BOOL) {
        return true;
    }
    if (tokens[lookahead].type == TokenType::ID) {
        return knownTypes.find(tokens[lookahead].value) != knownTypes.end();
    }
    return false;
}

Token Parser::peekNext() const {
    if (current + 1 >= tokens.size()) return Token(TokenType::END, "");
    return tokens[current + 1];
}

