#pragma once

#include <string>
#include <map>


enum class TokenType {
    // Операторы и логические/Символьные <operator>
    PLUS, MINUS, STAR, SLASH, PERCENT, // + - * / %
    SCREAMER, STRAIGHTLINE, RTRIBRACE, LTRIBRACE, // ! | > <
    EQUAL, WHY_SIGN , // = ?
    AMPERSAND, // &

    //Логические 
    INCREMENT,DECREMENT,LOGICAL_AND, LOGICAL_OR, // ++  --  && ||
    LOGICAL_EQUAL, NOT_EQUAL, GREATER_EQUAL , LESS_EQUAL, // ==  !=  >= <=
    PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, // +=  -= *= /= %=
    
    
    // Скобки и разделители <punctuator>
    LBRACE, RBRACE,         // ( )
    LFIGUREBRACE, RFIGUREBRACE, // { }
    LSQUAREBRACE, RSQUAREBRACE, // [ ]
    DOT, COMMA, SEMICOLON, DOTDOT,  // . , ; :
    
    
    // Лексемы <id> &&  <literal>
    ID,

    //<int_lit>, <float_lit>, <char_lit>, <str_lit>
    INT_LIT, FLOAT_LIT, CHAR_LIT, STR_LIT,   // числа, числа с точкой , 'строка char ' , "строка string"

    // Типы <type>
    INT, DOUBLE, CHAR, BOOL, VOID,

    //Командлеты
    IF , ELSE , FOR, WHILE, DO ,RETURN , BREAK , 
    CONTINUE , SIZEOF, CONST , STATIC_ASSERT, ASSERT, EXIT,

    NEWLINE, TABULATION, BACKSLASH, SQUOTE, DQUOTE, NULL_CHAR , //   \n, \t, \\ , \' , \" , \0
    COMMENT_STR, COMMENT_MULSTR_R, COMMENT_MULSTR_L,  // // , /*, */ 

    STRUCT, PRINT, READ,

    END
    
};


static std::string tokenTypeToString(TokenType type) {
    // Маппинг TokenType на строки
    static const std::map<TokenType, std::string> tokenTypeMap = {
        // Операторы и логические/символьные
        {TokenType::PLUS, "+"},
        {TokenType::MINUS, "-"},
        {TokenType::STAR, "*"},
        {TokenType::SLASH, "/"},
        {TokenType::PERCENT, "%"},
        {TokenType::SCREAMER, "!"},
        {TokenType::STRAIGHTLINE, "|"},
        {TokenType::RTRIBRACE, ">"},
        {TokenType::LTRIBRACE, "<"},
        {TokenType::EQUAL, "="},
        {TokenType::WHY_SIGN, "?"},
        {TokenType::INCREMENT, "++"},
        {TokenType::DECREMENT, "--"},
        {TokenType::LOGICAL_AND, "&&"},
        {TokenType::LOGICAL_OR, "||"},
        {TokenType::LOGICAL_EQUAL, "=="},
        {TokenType::NOT_EQUAL, "!="},
        {TokenType::GREATER_EQUAL, ">="},
        {TokenType::LESS_EQUAL, "<="},
        {TokenType::PLUS_ASSIGN, "+="},
        {TokenType::MINUS_ASSIGN, "-="},
        {TokenType::MULT_ASSIGN, "*="},
        {TokenType::DIV_ASSIGN, "/="},
        {TokenType::MOD_ASSIGN, "%="},
        {TokenType::AMPERSAND, "&"},

        // Скобки и разделители
        {TokenType::LBRACE, "("},
        {TokenType::RBRACE, ")"},
        {TokenType::LFIGUREBRACE, "{"},
        {TokenType::RFIGUREBRACE, "}"},
        {TokenType::LSQUAREBRACE, "["},
        {TokenType::RSQUAREBRACE, "]"},
        {TokenType::DOT, "."},
        {TokenType::COMMA, ","},
        {TokenType::SEMICOLON, ";"},
        {TokenType::DOTDOT, ":"},

        // Лексемы
        {TokenType::ID, "ID"},
        {TokenType::INT_LIT, "INT_LIT"},
        {TokenType::FLOAT_LIT, "FLOAT_LIT"},
        {TokenType::CHAR_LIT, "CHAR_LIT"},
        {TokenType::STR_LIT, "STR_LIT"},

        // Типы
        {TokenType::INT, "INT"},
        {TokenType::DOUBLE, "DOUBLE"},
        {TokenType::CHAR, "CHAR"},
        {TokenType::BOOL, "BOOL"},
        {TokenType::VOID, "VOID"},

        // Командлеты
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::FOR, "FOR"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::DO, "DO"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::BREAK, "BREAK"},
        {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::SIZEOF, "SIZEOF"},
        {TokenType::STATIC_ASSERT, "STATIC_ASSERT"},
        {TokenType::ASSERT, "ASSERT"},
        {TokenType::EXIT, "EXIT"},

        // Специальные символы
        {TokenType::NEWLINE, "\n"},
        {TokenType::TABULATION, "\t"},
        {TokenType::BACKSLASH, "\\"},
        {TokenType::SQUOTE, "\'"},
        {TokenType::DQUOTE, "\""},
        {TokenType::NULL_CHAR, "\0"},
        {TokenType::COMMENT_STR, "comment"},
        {TokenType::COMMENT_MULSTR_R, "/*"},
        {TokenType::COMMENT_MULSTR_L, "*/"},

        // Структуры
        {TokenType::STRUCT, "STRUCT"},
        { TokenType::PRINT, "PRINT"},
        {TokenType::READ, "READ" },

        // Завершение
        {TokenType::END, "END"}
    };

    // Используем map для получения строки
    auto it = tokenTypeMap.find(type);
    if (it != tokenTypeMap.end()) {
        return it->second;
    }
    
    return "UNKNOWN"; // Если не найдено, возвращаем UNKNOWN
}




struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType type , std::string value = ""):
    type(type), value(std::move(value)){}

    bool operator==(TokenType type) const {
        return this->type == type;
    }

    std::string toString() const {
        return "Token(" + tokenTypeToString(type) + ", \"" + value + "\")";
    }
};

