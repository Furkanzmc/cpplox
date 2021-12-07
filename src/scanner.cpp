#include "scanner.h"
#include "log.h"

namespace {
bool is_at_end(const scanner& scn)
{
    return scn.current >= scn.source.size();
}

char advance(scanner& scn)
{
    return scn.source.at(scn.current++);
}

char peek(scanner& scn)
{
    if (is_at_end(scn)) {
        return '\0';
    }

    return scn.source.at(scn.current);
}

char peek_next(scanner& scn)
{
    if (scn.current + 1 >= scn.source.size()) {
        return '\0';
    }

    return scn.source.at(scn.current + 1);
}

bool match(scanner& scn, char expected)
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

token create_token(const scanner& scn, token::token_type type, void* literal)
{
    return {
        type, scn.source.substr(scn.start, scn.current), literal, scn.line
    };
}

void scan_string(scanner& scn)
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

    std::string_view str = scn.source.substr(scn.start + 1, scn.current - 1);
    scn.tokens.push_back(
      token{ token::token_type::STRING, std::move(str), nullptr, scn.line });
}

void scan_number(scanner& scn)
{
    while (std::isdigit(peek(scn))) {
        advance(scn);
    }

    if (peek(scn) == '.' && std::isdigit(peek_next(scn))) {
        while (std::isdigit(peek(scn))) {
            advance(scn);
        }
    }

    scn.tokens.push_back(token{ token::token_type::NUMBER,
                                scn.source.substr(scn.start, scn.current),
                                nullptr,
                                scn.line });
}

void scan_token(scanner& scn)
{
    char ch = advance(scn);
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
            else {
                log_error(scn.line, "Unknown token.");
            }
    }
}
}

scanner scan_tokens(scanner scn)
{
    while (!is_at_end(scn)) {
        scn.start = scn.current;
        scan_token(scn);
    }

    scn.tokens.push_back(
      token{ token::token_type::LEFT_PAREN, "", nullptr, scn.current });
    return scn;
}
