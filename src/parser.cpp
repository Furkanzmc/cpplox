#include "parser.h"

#include "expr.h"
#include "utils.h"

#include <optional>
#include <exception>
#include <sstream>

#ifndef LOX_EXCEPTION_ENABLED
#error "Parser relies on exceptions to be enabled."
#endif

using token_type = lox::token::token_type;

template<typename T>
using expr_h = std::unique_ptr<T>;

namespace {
struct parser_data {
    const std::vector<lox::token>& tokens;
    std::size_t current{ 0 };
};

class parser_error : public std::exception {
};

lox::expr parse_ternary(parser_data& data);

bool is_at_end(const parser_data& data) LOX_NOEXCEPT
{
    return data.current == data.tokens.size();
}

[[nodiscard]] const lox::token& peek(const parser_data& data) LOX_NOEXCEPT
{
    assert(data.current >= 0 && data.current < data.tokens.size());
    return data.tokens.at(data.current);
}

[[nodiscard]] const lox::token& previous(const parser_data& data) LOX_NOEXCEPT
{
    assert(data.current > 0);
    return data.tokens.at(data.current - 1);
}

[[maybe_unused]] const lox::token& advance(parser_data& data) LOX_NOEXCEPT
{
    if (!is_at_end(data)) {
        data.current++;
    }

    assert(data.current <= data.tokens.size());
    return previous(data);
}

[[nodiscard]] bool check(const parser_data& data, token_type type) LOX_NOEXCEPT
{
    if (is_at_end(data)) {
        return false;
    }

    return peek(data).type == type;
}

void log_error(const lox::token& token, std::string_view message)
{
    lox::log_error(token.line_str, token.line, token.column_end, message);
    throw parser_error{};
}

#if 0
void synchronize(parser_data& data) LOX_NOEXCEPT
{
    static const std::vector<token_type> statements{
        token_type::CLASS,
        token_type::FUN,
        token_type::VAR,
        token_type::FOR,
        token_type::IF,
        token_type::WHILE,
        token_type::PRINT,
        token_type::RETURN,
    };

    advance(data);
    while (!is_at_end(data)) {
        if (previous(data).type == token_type::SEMICOLON) {
            return;
        }

        if (std::find(statements.cbegin(),
              statements.cend(),
              peek(data).type) != statements.cend()) {
            return;
        }
    }
}
#endif

const lox::token& consume(parser_data& data,
  token_type type,
  std::string_view error_message) LOX_NOEXCEPT
{
    if (!check(data, type)) {
        log_error(peek(data), error_message);
    }

    return advance(data);
}

bool match(parser_data& data,
  std::initializer_list<token_type> tokens) LOX_NOEXCEPT
{
    for (const auto type : tokens) {
        if (check(data, type)) {
            advance(data);
            return true;
        }
    }

    return false;
}

lox::expr parse_primary(parser_data& data)
{
    if (match(data, { token_type::NIL })) {
        return expr_h<lox::literal>{ new lox::literal{ nullptr } };
    }

    if (match(data, { token_type::FALSE })) {
        return expr_h<lox::literal>{ new lox::literal{ lox::object{ false } } };
    }

    if (match(data, { token_type::TRUE })) {
        return expr_h<lox::literal>{ new lox::literal{ lox::object{ true } } };
    }

    if (match(data, { token_type::NUMBER, token_type::STRING })) {
        return expr_h<lox::literal>{ new lox::literal{
          previous(data).literal } };
    }

    if (match(data, { token_type::LEFT_PAREN })) {
        auto expression = parse_ternary(data);
        consume(
          data, token_type::RIGHT_PAREN, "Expected ')' after expression '('.");
        return expr_h<lox::grouping>{ new lox::grouping{
          std::move(expression) } };
    }

    if (data.current > 0 && previous(data).type == token_type::EQUAL_EQUAL) {
        log_error(previous(data), "Unterminated comparison.");
    }

    return {};
}

lox::expr parse_unary(parser_data& data)
{
    if (match(data, { token_type::BANG, token_type::MINUS })) {
        const lox::token& opr = previous(data);
        auto right = parse_unary(data);
        return expr_h<lox::unary>{ new lox::unary{ opr, std::move(right) } };
    }

    return parse_primary(data);
}

lox::expr parse_factor(parser_data& data)
{
    auto expression = parse_unary(data);
    while (match(data, { token_type::SLASH, token_type::STAR })) {
        const lox::token& opr = previous(data);
        auto right = parse_unary(data);
        expression = expr_h<lox::binary>{ new lox::binary{
          std::move(expression), opr, std::move(right) } };
    }

    return expression;
}

lox::expr parse_term(parser_data& data)
{
    auto expression = parse_factor(data);
    while (match(data, { token_type::MINUS, token_type::PLUS })) {
        const lox::token& opr = previous(data);
        auto right = parse_factor(data);
        expression = expr_h<lox::binary>{ new lox::binary{
          std::move(expression), opr, std::move(right) } };
    }

    return expression;
}

lox::expr parse_comparison(parser_data& data)
{
    auto expression = parse_term(data);

    static const std::initializer_list<token_type> types{ token_type::GREATER,
        token_type::GREATER_EQUAL,
        token_type::LESS,
        token_type::LESS_EQUAL };

    while (match(data, types)) {
        const lox::token& opr = previous(data);
        auto right = parse_term(data);
        expression = expr_h<lox::binary>{ new lox::binary{
          std::move(expression), opr, std::move(right) } };
    }

    return expression;
}

lox::expr parse_equality(parser_data& data)
{
    auto expression = parse_comparison(data);
    while (match(data, { token_type::BANG_EQUAL, token_type::EQUAL_EQUAL })) {
        const lox::token& opr = previous(data);
        lox::expr right = parse_comparison(data);
        expression = expr_h<lox::binary>{ new lox::binary{
          std::move(expression), opr, std::move(right) } };
    }

    return expression;
}

lox::expr parse_expression(parser_data& data)
{
    return parse_equality(data);
}

lox::expr parse_ternary(parser_data& data)
{
    std::vector<lox::expr> exprs;
    exprs.push_back(parse_expression(data));
    const auto push = [&exprs](auto expr) {
        if (std::holds_alternative<std::unique_ptr<lox::ternary>>(expr)) {
            exprs.push_back(
              expr_h<lox::grouping>{ new lox::grouping{ std::move(expr) } });
        }
        else {
            exprs.push_back(std::move(expr));
        }
    };

    if (match(data, { token_type::QUESTION_MARK })) {
        push(parse_ternary(data));

        if (match(data, { token_type::COLON })) {
            push(parse_ternary(data));
        }
    }

    if (exprs.size() == 1) {
        return std::move(exprs.front());
    }

    if (exprs.size() != 3) {
        log_error(peek(data), "Expected ':' to finish the ternary operator.");
        return expr_h<lox::ternary>{ new lox::ternary{
          std::move(exprs[0]), std::move(exprs[1]), {} } };
    }

    assert(exprs.size() == 3);
    return expr_h<lox::ternary>{ new lox::ternary{
      std::move(exprs[0]), std::move(exprs[1]), std::move(exprs[2]) } };
}
}

lox::expr lox::parse(const std::vector<lox::token>& tokens) LOX_NOEXCEPT
{
    assert(!tokens.empty());

    parser_data data{ tokens, 0 };
    try {
        return parse_ternary(data);
    }
    catch (parser_error&) {
        return {};
    }
}
