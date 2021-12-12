#include "scanner.h"
#include "log.h"

#include <map>
#include <string_view>

namespace {
const std::map<std::string_view, token::token_type> s_keywords{
    { "and", token::token_type::AND },
    { "class", token::token_type::CLASS },
    { "else", token::token_type::ELSE },
    { "false", token::token_type::FALSE },
    { "for", token::token_type::FOR },
    { "fun", token::token_type::FUN },
    { "if", token::token_type::IF },
    { "nil", token::token_type::NIL },
    { "or", token::token_type::OR },
    { "print", token::token_type::PRINT },
    { "super", token::token_type::SUPER },
    { "this", token::token_type::THIS },
    { "true", token::token_type::TRUE },
    { "var", token::token_type::VAR },
    { "while", token::token_type::WHILE },
};

[[nodiscard]] bool is_at_end(const scanner& scn) LOX_NOEXCEPT
{
    return scn.current >= scn.source.size();
}

char advance(scanner& scn) LOX_NOEXCEPT
{
    return scn.source.at(scn.current++);
}

[[nodiscard]] char peek(scanner& scn) LOX_NOEXCEPT
{
    if (is_at_end(scn)) {
        return '\0';
    }

    return scn.source.at(scn.current);
}

[[nodiscard]] char peek_next(scanner& scn) LOX_NOEXCEPT
{
    if (scn.current + 1 >= scn.source.size()) {
        return '\0';
    }

    return scn.source.at(scn.current + 1);
}

[[nodiscard]] bool match(scanner& scn, char expected) LOX_NOEXCEPT
{
    if (is_at_end(scn)) {
        return false;
    }

    if (scn.source.at(scn.current) != expected) {
        return false;
    }

    scn.current++;
    return true;
}

[[nodiscard]] token create_token(const scanner& scn,
  token::token_type type,
  void* literal) LOX_NOEXCEPT
{
    return { type,
        scn.source.substr(scn.start, scn.current - scn.start),
        literal,
        scn.line,
        scn.start,
        scn.current };
}

void scan_string(scanner& scn) LOX_NOEXCEPT
{
    while (peek(scn) != '"' && !is_at_end(scn)) {
        if (peek(scn) == '\n') {
            scn.line++;
        }

        advance(scn);
    }

    if (is_at_end(scn)) {
        log_error(scn.line, "Unterminated string.");
        return;
    }

    // Closing '"'
    advance(scn);

    scn.tokens.push_back(token{ token::token_type::STRING,
      scn.source.substr(scn.start + 1, scn.current - scn.start - 2),
      nullptr,
      scn.line,
      scn.start,
      scn.current });
}

void scan_number(scanner& scn) LOX_NOEXCEPT
{
    // First character was consumed before calling scan_number.
    while (std::isdigit(peek(scn)) ||
           (peek(scn) == '.' && std::isdigit(peek_next(scn)))) {
        advance(scn);
    }

    const auto num_str = scn.source.substr(scn.start, scn.current - scn.start);
    scn.tokens.push_back(token{ token::token_type::NUMBER,
      std::stod(std::string{ num_str }),
      nullptr,
      scn.line,
      scn.start,
      scn.current });
}

void scan_identifier(scanner& scn) LOX_NOEXCEPT
{
    while (std::isalnum(peek(scn))) {
        advance(scn);
    }

    std::string_view text{ scn.source.substr(
      scn.start, scn.current - scn.start) };
    const auto foundIt = s_keywords.find(text);
    if (foundIt != s_keywords.cend()) {
        scn.tokens.push_back(token{ token::token_type::IDENTIFIER,
          text,
          nullptr,
          scn.line,
          scn.start,
          scn.current });
    }
}

void scan_tokens_impl(scanner& scn) LOX_NOEXCEPT
{
    const char ch = advance(scn);
    switch (ch) {
        case '(':
            scn.tokens.push_back(
              create_token(scn, token::token_type::LEFT_PAREN, nullptr));
            break;
        case ')':
            scn.tokens.push_back(
              create_token(scn, token::token_type::RIGHT_PAREN, nullptr));
            break;
        case '{':
            scn.tokens.push_back(
              create_token(scn, token::token_type::LEFT_BRACE, nullptr));
            break;
        case '}':
            scn.tokens.push_back(
              create_token(scn, token::token_type::RIGHT_BRACE, nullptr));
            break;
        case ',':
            scn.tokens.push_back(
              create_token(scn, token::token_type::COMMA, nullptr));
            break;
        case '.':
            scn.tokens.push_back(
              create_token(scn, token::token_type::DOT, nullptr));
            break;
        case '-':
            scn.tokens.push_back(
              create_token(scn, token::token_type::MINUS, nullptr));
            break;
        case '+':
            scn.tokens.push_back(
              create_token(scn, token::token_type::PLUS, nullptr));
            break;
        case ';':
            scn.tokens.push_back(
              create_token(scn, token::token_type::SEMICOLON, nullptr));
            break;
        case '*':
            scn.tokens.push_back(
              create_token(scn, token::token_type::STAR, nullptr));
            break;
        case '!':
            scn.tokens.push_back(
              match(scn, '=')
                ? create_token(scn, token::token_type::BANG_EQUAL, nullptr)
                : create_token(scn, token::token_type::BANG, nullptr));
            break;
        case '=':
            scn.tokens.push_back(
              match(scn, '=')
                ? create_token(scn, token::token_type::EQUAL_EQUAL, nullptr)
                : create_token(scn, token::token_type::EQUAL, nullptr));
            break;
        case '<':
            scn.tokens.push_back(
              match(scn, '=')
                ? create_token(scn, token::token_type::LESS_EQUAL, nullptr)
                : create_token(scn, token::token_type::LESS, nullptr));
            break;
        case '>':
            scn.tokens.push_back(
              match(scn, '=')
                ? create_token(scn, token::token_type::GREATER_EQUAL, nullptr)
                : create_token(scn, token::token_type::GREATER, nullptr));
            break;
        case '/':
            if (match(scn, '/')) {
                while (peek(scn) != '\n' && !is_at_end(scn)) {
                    advance(scn);
                }
            }
            else {
                scn.tokens.push_back(
                  create_token(scn, token::token_type::SLASH, nullptr));
            }
        case ' ':
            [[fallthrough]];
        case '\r':
            [[fallthrough]];
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            scn.line++;
            break;
        case '"':
            scan_string(scn);
            break;
        default:
            if (std::isdigit(ch)) {
                scan_number(scn);
            }
            else if (std::isalpha(ch)) {
                scan_identifier(scn);
            }
            else {
                log_error(scn.line, "Unknown token.");
            }
    }
}
}

scanner scan_tokens(std::string_view source) LOX_NOEXCEPT
{
    scanner scn{ source };
    while (!is_at_end(scn)) {
        scn.start = scn.current;
        scan_tokens_impl(scn);
    }

    scn.tokens.push_back(token{ token::token_type::END_OF_FILE,
      "",
      nullptr,
      scn.line,
      scn.current,
      scn.current });
    return scn;
}
