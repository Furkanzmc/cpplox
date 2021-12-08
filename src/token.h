#ifndef TOKEN_H
#define TOKEN_H

#include <string_view>
#include <iostream>
#include <variant>

struct token {

    enum class token_type {
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

inline std::ostream& operator<<(std::ostream& os, token::token_type type)
{
    switch (type) {
        case token::token_type::LEFT_PAREN:
            os << "LEFT_PAREN";
            break;
        case token::token_type::RIGHT_PAREN:
            os << "RIGHT_PAREN";
            break;
        case token::token_type::LEFT_BRACE:
            os << "LEFT_BRACE";
            break;
        case token::token_type::RIGHT_BRACE:
            os << "RIGHT_BRACE";
            break;
        case token::token_type::COMMA:
            os << "COMMA";
            break;
        case token::token_type::DOT:
            os << "DOT";
            break;
        case token::token_type::MINUS:
            os << "MINUS";
            break;
        case token::token_type::PLUS:
            os << "PLUS";
            break;
        case token::token_type::SEMICOLON:
            os << "SEMICOLON";
            break;
        case token::token_type::SLASH:
            os << "SLASH";
            break;
        case token::token_type::STAR:
            os << "STAR";
            break;
        case token::token_type::BANG:
            os << "BANG";
            break;
        case token::token_type::BANG_EQUAL:
            os << "BANG_EQUAL";
            break;
        case token::token_type::EQUAL:
            os << "EQUAL";
            break;
        case token::token_type::EQUAL_EQUAL:
            os << "EQUAL_EQUAL";
            break;
        case token::token_type::GREATER:
            os << "GREATER";
            break;
        case token::token_type::GREATER_EQUAL:
            os << "GREATER_EQUAL";
            break;
        case token::token_type::LESS:
            os << "LESS";
            break;
        case token::token_type::LESS_EQUAL:
            os << "LESS_EQUAL";
            break;
        case token::token_type::IDENTIFIER:
            os << "IDENTIFIER";
            break;
        case token::token_type::STRING:
            os << "STRING";
            break;
        case token::token_type::NUMBER:
            os << "NUMBER";
            break;
        case token::token_type::AND:
            os << "AND";
            break;
        case token::token_type::CLASS:
            os << "CLASS";
            break;
        case token::token_type::ELSE:
            os << "ELSE";
            break;
        case token::token_type::FALSE:
            os << "FALSE";
            break;
        case token::token_type::FUN:
            os << "FUN";
            break;
        case token::token_type::FOR:
            os << "FOR";
            break;
        case token::token_type::IF:
            os << "IF";
            break;
        case token::token_type::NIL:
            os << "NIL";
            break;
        case token::token_type::OR:
            os << "OR";
            break;
        case token::token_type::PRINT:
            os << "PRINT";
            break;
        case token::token_type::RETURN:
            os << "RETURN";
            break;
        case token::token_type::SUPER:
            os << "SUPER";
            break;
        case token::token_type::THIS:
            os << "THIS";
            break;
        case token::token_type::TRUE:
            os << "TRUE";
            break;
        case token::token_type::VAR:
            os << "VAR";
            break;
        case token::token_type::WHILE:
            os << "WHILE";
            break;
        case token::token_type::END_OF_FILE:
            os << "END_OF_FILE";
            break;
        default:
            os << "UNKNOWN";
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const token& tk)
{
    os << tk.type << ": ";
    if (std::holds_alternative<double>(tk.lexeme)) {
        os << std::get<double>(tk.lexeme);
    }
    else {
        os << std::get<std::string_view>(tk.lexeme);
    }

    os << " -> "
       << "L" << tk.line << ", C" << tk.column_start << ":" << tk.column_end;

    return os;
}

#endif
