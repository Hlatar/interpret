#include "../inc/lexer.hpp"
#include <stdexcept>
#include <iostream>


Lexer::Lexer(const std::string& input) : input(input) {}

const std::unordered_map<std::string, TokenType> Lexer::escape{
    {"\"", TokenType::DQUOTE},
    {"\'", TokenType::SQUOTE},
    {"\\", TokenType::BACKSLASH},
    {"n", TokenType::NEWLINE},
    {"t", TokenType::TABULATION},
    {"0", TokenType::NULL_CHAR}
};

const std::unordered_map<std::string, TokenType> Lexer::operators = {
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::STAR},
    {"/", TokenType::SLASH},
    {"%", TokenType::PERCENT},
    {"!", TokenType::SCREAMER},
    {"|", TokenType::STRAIGHTLINE},
    {">", TokenType::RTRIBRACE},
    {"<", TokenType::LTRIBRACE},
    {"=", TokenType::EQUAL},
    {"?", TokenType::WHY_SIGN},
    {"(", TokenType::LBRACE},
    {")", TokenType::RBRACE},
    {"{", TokenType::LFIGUREBRACE},
    {"}", TokenType::RFIGUREBRACE},
    {"[", TokenType::LSQUAREBRACE},
    {"]", TokenType::RSQUAREBRACE},
    {".", TokenType::DOT},
    {",", TokenType::COMMA},    
    {";", TokenType::SEMICOLON},
    {":", TokenType::DOTDOT},
    {"++", TokenType::INCREMENT},
    {"--", TokenType::DECREMENT},
    {"&&", TokenType::LOGICAL_AND}, 
    {"||", TokenType::LOGICAL_OR},
    {"==", TokenType::LOGICAL_EQUAL}, 
    {"!=", TokenType::NOT_EQUAL}, 
    {">=", TokenType::GREATER_EQUAL},
    {"<=", TokenType::LESS_EQUAL}, 
    {"+=", TokenType::PLUS_ASSIGN},
    {"-=", TokenType::MINUS_ASSIGN}, 
    {"*=", TokenType::MULT_ASSIGN}, 
    {"/=", TokenType::DIV_ASSIGN},
    {"%=", TokenType::MOD_ASSIGN},
    {"\"", TokenType::DQUOTE},
    {"\'", TokenType::SQUOTE},
    {"\\", TokenType::BACKSLASH},
    {"n", TokenType::NEWLINE},
    {"t", TokenType::TABULATION},
    {"//", TokenType::COMMENT_STR},
    {"/*", TokenType::COMMENT_MULSTR_R},
    {"*/", TokenType::COMMENT_MULSTR_L},
    {"&", TokenType::AMPERSAND},
    {"->", TokenType::ARROW},
    {"::", TokenType::SCOPE},
};

//лексер и ascape последователь игнорить 

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"if", TokenType::IF}, 
    {"else", TokenType::ELSE}, 
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR}, 
    {"return", TokenType::RETURN}, 
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"int", TokenType::INT}, 
    {"double", TokenType::DOUBLE},
    {"char", TokenType::CHAR}, 
    {"bool", TokenType::BOOL}, 
    {"void", TokenType::VOID},
    {"short", TokenType::SHORT},
    {"long", TokenType::LONG},
    {"float", TokenType::FLOAT},
    {"sizeof", TokenType::SIZEOF}, 
    {"const", TokenType::CONST},
    {"unsigned", TokenType::UNSIGNED}, 
    {"static_assert", TokenType::STATIC_ASSERT},
    {"assert", TokenType::ASSERT}, 
    {"exit", TokenType::EXIT},
    {"struct", TokenType::STRUCT},
    {"do", TokenType::DO},
    {"sizeof", TokenType::SIZEOF},
    {"static_assert", TokenType::STATIC_ASSERT},
    {"assert", TokenType::ASSERT},
    {"exit", TokenType::EXIT},
    {"print" , TokenType::PRINT},
    {"read" , TokenType::READ},
    {"namespace", TokenType::NAMESPACE}
};




std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (index < input.size()) {
        Token token = extract();
        tokens.push_back(token);
        if (token.type == TokenType::COMMENT_STR) {
            tokens.erase(tokens.end()); // Прерываем после END
        }
        if (token.type == TokenType::END) {
            break; // Прерываем после END
        }
    }
    // Гарантируем, что последний токен — END
    if (tokens.empty() || tokens.back().type != TokenType::END) {
        tokens.push_back(Token(TokenType::END, ""));
    }
    return tokens;
}

Token Lexer::extract() {
    while (index < input.size() && std::isspace(input[index])) ++index;

    if (index >= input.size() ) {
        return {TokenType::END, ""};
    }


    if (std::isdigit(input[index])) {
        return extract_number();
    }

    if (std::isalpha(input[index]) || input[index] == '_') {
        return extract_identifier();
    }
    
    if (operators.contains(std::string(1, input[index])) || escape.contains(std::string(1, input[index]))) {
        return extract_operator();
    }

    

    std::cerr << "error: invalid character '" << input[index]<<"' in identifier " << std::endl;
    exit(1); 
    // throw std::runtime_error("Unexpected operator: " + op);
}


Token Lexer::extract_number() { // для чисел прописано
    int flag_float = 0;
    std::size_t size = 0;

    while (index + size < input.size() && std::isdigit(input[index + size])) ++size;


    if (input[index + size] == '.') {
        flag_float = 1;
        ++size;
        while (index + size < input.size() && std::isdigit(input[index + size])) ++size;

    }

    std::string value(input, index, size);
    index += size;
    if(flag_float == 0 ){
        return {TokenType::INT_LIT, value};
    }else{
        return {TokenType::FLOAT_LIT, value};
    }
    throw std::runtime_error("Unexpected digit \n"); // std::cerr << "The unknown type of digit" << std::endl;
}

Token Lexer::extract_identifier() {
    std::size_t size = 0;
    
    
    while (index + size < input.size() && std::isalnum(input[index + size]) || input[index + size] == '_') ++size; 


    std::string name(input, index, size);
    index += size;


    auto it = keywords.find(name);
    if (it != keywords.end()) {
        return {it->second, name}; // Нашли ключевое слово
    }
    return {TokenType::ID, name};  // Иначе это идентификатор    
    throw std::runtime_error("Unexpected identifier \n");
}


Token Lexer::extract_operator() {
    std::size_t size = 1;  // Начинаем с одного символа
    std::size_t size_tmp = 0;
    int flag = 0;
    std::string op(1, input[index]); // Первый символ оператора

    if(op == "\""){  
        std::size_t i = 0;
        index++;
        while(index + i < input.size() && input[index + i] != '"'){
            if(input[index + i] == '\\'){
                std::cout << "Spotted the amongus: " << input[index + i] << std::endl;
                
                if(escape.contains(std::string(1 , input[index + i + 1]))){
                    if(input[index + i + 1] == '0' ){
                        input.replace(index + i + 1, 1 , std::string(1, '\0'));
                    }else if(input[index + i + 1] == 't'){
                        input.replace(index + i + 1, 1 , std::string(1, '\t'));
                    }else if(input[index + i + 1] == 'n'){
                        input.replace(index + i + 1, 1 , std::string(1, '\n'));
                    }
                    input.erase(index+ i, 1);
                }else{
                    std::cerr << "error: missing terminating \' character"  << std::endl;
                    exit(1);
                }
            }
            ++i;
        }

        if(input.size() == index + i){
            std::cerr << "error: missing terminating \" character"  << std::endl;
            exit(1); 
        }else{
            //std::cout << index + i<< std::endl;
            std::string name(input, index, i);
            index += i +1;
            return {TokenType::STR_LIT, name};
        }
        
    }

    if(op == "\'"){  
        std::size_t i = 0;
        index++;

        while(index + i < input.size() && input[index + i] != '\''){
            if(input[index+ i] == '\\' && input[index+ i + 1 ] == '\'' ){
                i += 2;
                continue;
            }else if (input[index+ i] == '\\' && input[index+ i + 1 ] == '\\' ) {
                i += 2;
                continue;
            }
            ++i;
        }

        //std::cout << input[index + i] << std::endl;
        
        if(input.size() == index + i){
            std::cerr << "error: missing terminating \' character"  << std::endl;
            exit(1); 
        }else if(i == 0){
            std::cerr << "error: empty character constant"  << std::endl;
            exit(1); 
        }else if( (i > 1 && input[index] != '\\') || (i > 2 && input[index] == '\\')){
            std::cerr << "warning: character constant too long for its type"  << std::endl;
            exit(1); 
        }else if (input[index] == '\\'){
            std::string name(input, index, i);
            index += i +1;
            
            auto it = escape.find(std::string(1, name[1]));
            if (it != escape.end()) {
                if(std::string(1, name[1]) == "0"){
                    name = std::string(1, '\0');
                }else if(std::string(1, name[1]) == "n"){
                    name = std::string(1, '\n');
                }else if(std::string(1, name[1]) == "t"){
                    name = std::string(1, '\t');
                }else if(std::string(1, name[1]) == "\""){
                    name = std::string(1, '\"');
                }else if(std::string(1, name[1]) == "\'"){
                    name = std::string(1, '\'');
                }else if(std::string(1, name[1]) == "\\"){
                    name = std::string(1, '\\');
                }
                return {TokenType::CHAR_LIT, name}; // Нашли ключевое слово
            }else{
                std::cerr << "warning: unknown escape sequence: '"<< name << "'"  << std::endl;
                exit(1); 
            }
            throw std::runtime_error("Unexpected operator: " + op);
        }else{
            std::string name(input, index, i);
            index += i +1;
            //std::cout << name<< std::endl;
            return {TokenType::CHAR_LIT, name};
        }
        throw std::runtime_error("Unexpected operator: " + op);
    }

    // проверяем, есть ли следующий символ в `operators`
    if (index + 1 < input.size()) {
        std::string two_char_op = op + input[index + 1]; // двухсимвольный оператор
        if (operators.contains(two_char_op)) {
            op = two_char_op;
            size = 2;
        }
    }

    // проверяем, есть ли оператор в словаре
    auto it = operators.find(op);
    if(op == "//"){
        std::size_t i = 0;
        while(index + i < input.size() && input[index + i] != '\n'){
            ++i;
        }
        std::string comment(input, index , i);
        index += size + i - 1 ;
        

        return{TokenType::COMMENT_STR , comment};

    }

    if (op == "/*") {
        std::size_t i = 2;
    
        // Ищем закрытие */ безопасно
        while (index + i + 1 < input.size()) {
            if (input[index + i] == '*' && input[index + i + 1] == '/') {
                i += 2;  // включаем */ в длину комментария
                break;
            }
            ++i;
        }
    
        // Если не нашли закрытие комментария
        if (index + i + 1 == input.size()) {
            throw std::runtime_error("Ошибка: незавершённый многострочный комментарий");
        }
    
        std::string comment = input.substr(index, i);
        index += i;
    
        return {TokenType::COMMENT_STR, comment};
    }
    
    if (op == "*/") {
        throw std::runtime_error("Ошибка: неожиданное закрытие комментария '*/'");
    }
    

    if (it != operators.end()) {
        index += size;  // Увеличиваем индекс на длину оператора
        return {it->second, op};
    }
    
    throw std::runtime_error("Unexpected operator: " + op);
}

/// для операторов лексер доибть 
