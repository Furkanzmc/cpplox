#include "token.h"

#include <map>

namespace {
using token_type = lox::token::token_type;

const std::map<token_type, std::string_view> s_type_names{
    { token_type::LEFT_PAREN, "LEFT_PAREN" },
    { token_type::RIGHT_PAREN, "RIGHT_PAREN" },
    { token_type::LEFT_BRACE, "LEFT_BRACE" },
    { token_type::RIGHT_BRACE, "RIGHT_BRACE" },
    { token_type::COMMA, "COMMA" },
    { token_type::DOT, "DOT" },
    { token_type::MINUS, "MINUS" },
    { token_type::PLUS, "PLUS" },
    { token_type::SEMICOLON, "SEMICOLON" },
    { token_type::SLASH, "SLASH" },
    { token_type::STAR, "STAR" },
    { token_type::BANG, "BANG" },
    { token_type::BANG_EQUAL, "BANG_EQUAL" },
    { token_type::EQUAL, "EQUAL" },
    { token_type::EQUAL_EQUAL, "EQUAL_EQUAL" },
    { token_type::GREATER, "GREATER" },
    { token_type::GREATER_EQUAL, "GREATER_EQUAL" },
    { token_type::LESS, "LESS" },
    { token_type::LESS_EQUAL, "LESS_EQUAL" },
    { token_type::IDENTIFIER, "IDENTIFIER" },
    { token_type::STRING, "STRING" },
    { token_type::NUMBER, "NUMBER" },
    { token_type::AND, "AND" },
    { token_type::CLASS, "CLASS" },
    { token_type::ELSE, "ELSE" },
    { token_type::FALSE, "FALSE" },
    { token_type::FUN, "FUN" },
    { token_type::FOR, "FOR" },
    { token_type::IF, "IF" },
    { token_type::NIL, "NIL" },
    { token_type::OR, "OR" },
    { token_type::PRINT, "PRINT" },
    { token_type::RETURN, "RETURN" },
    { token_type::SUPER, "SUPER" },
    { token_type::THIS, "THIS" },
    { token_type::TRUE, "TRUE" },
    { token_type::VAR, "VAR" },
    { token_type::WHILE, "WHILE" },
    { token_type::COMMENT, "COMMENT" },
    { token_type::QUESTION_MARK, "QUESTION_MARK" },
    { token_type::COLON, "COLON" },
    { token_type::END_OF_FILE, "END_OF_FILE" }
};
}

std::ostream& operator<<(std::ostream& os, lox::token::token_type type)
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

std::ostream& operator<<(std::ostream& os, const lox::token& tk)
{
    os << tk.type << ": " << tk.lexeme << " -> "
       << "L" << tk.line << ", C" << tk.column_start << ":" << tk.column_end;

    return os;
}
