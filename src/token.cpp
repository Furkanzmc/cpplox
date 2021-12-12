#include "token.h"

#include <map>

namespace {
const std::map<token::token_type, std::string_view> s_type_names{
    { token::token_type::LEFT_PAREN, "LEFT_PAREN" },
    { token::token_type::RIGHT_PAREN, "RIGHT_PAREN" },
    { token::token_type::LEFT_BRACE, "LEFT_BRACE" },
    { token::token_type::RIGHT_BRACE, "RIGHT_BRACE" },
    { token::token_type::COMMA, "COMMA" },
    { token::token_type::DOT, "DOT" },
    { token::token_type::MINUS, "MINUS" },
    { token::token_type::PLUS, "PLUS" },
    { token::token_type::SEMICOLON, "SEMICOLON" },
    { token::token_type::SLASH, "SLASH" },
    { token::token_type::STAR, "STAR" },
    { token::token_type::BANG, "BANG" },
    { token::token_type::BANG_EQUAL, "BANG_EQUAL" },
    { token::token_type::EQUAL, "EQUAL" },
    { token::token_type::EQUAL_EQUAL, "EQUAL_EQUAL" },
    { token::token_type::GREATER, "GREATER" },
    { token::token_type::GREATER_EQUAL, "GREATER_EQUAL" },
    { token::token_type::LESS, "LESS" },
    { token::token_type::LESS_EQUAL, "LESS_EQUAL" },
    { token::token_type::IDENTIFIER, "IDENTIFIER" },
    { token::token_type::STRING, "STRING" },
    { token::token_type::NUMBER, "NUMBER" },
    { token::token_type::AND, "AND" },
    { token::token_type::CLASS, "CLASS" },
    { token::token_type::ELSE, "ELSE" },
    { token::token_type::FALSE, "FALSE" },
    { token::token_type::FUN, "FUN" },
    { token::token_type::FOR, "FOR" },
    { token::token_type::IF, "IF" },
    { token::token_type::NIL, "NIL" },
    { token::token_type::OR, "OR" },
    { token::token_type::PRINT, "PRINT" },
    { token::token_type::RETURN, "RETURN" },
    { token::token_type::SUPER, "SUPER" },
    { token::token_type::THIS, "THIS" },
    { token::token_type::TRUE, "TRUE" },
    { token::token_type::VAR, "VAR" },
    { token::token_type::WHILE, "WHILE" },
    { token::token_type::COMMENT, "COMMENT" },
    { token::token_type::END_OF_FILE, "END_OF_FILE" }
};
}

std::ostream& operator<<(std::ostream& os, token::token_type type)
{
    const auto foundIt = s_type_names.find(type);
    if (foundIt != s_type_names.cend()) {
        os << foundIt->second;
    }
    else {
        os << "UNKNOWN";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const token& tk)
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
