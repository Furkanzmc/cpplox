#ifndef TOKEN_H
#define TOKEN_H

#include <string_view>
#include <iostream>
#include <variant>

struct token {

    enum class token_type {
        COMMENT,

        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        END_OF_FILE
    };

    token_type type;
    std::variant<std::string_view, double> lexeme;
    void* literal;
    std::size_t line;
    std::size_t column_start;
    std::size_t column_end;
};

std::ostream& operator<<(std::ostream& os, token::token_type type);
std::ostream& operator<<(std::ostream& os, const token& tk);

#endif
