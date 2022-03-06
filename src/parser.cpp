#include "parser.h"

#include "expr.h"
#include "utils.h"

#include <optional>
#include <exception>
#include <sstream>

#ifndef LOX_EXCEPTION_ENABLED
#error "Parser relies on exceptions to be enabled."
#endif

namespace {
using expr_c = lox::copyable<lox::expr*>;

using token_type = lox::token::token_type;

struct parser_state {
    const std::vector<lox::token>& tokens;
    std::size_t current{ 0 };
};

class parse_error : public std::exception {
};

[[nodiscard]] lox::expr parse_ternary(parser_state& state) LOX_NOEXCEPT;

[[nodiscard]] bool is_at_end(const parser_state& state) LOX_NOEXCEPT
{
    return state.current == state.tokens.size();
}

[[nodiscard]] const lox::token& peek(const parser_state& state) LOX_NOEXCEPT
{
    assert(state.current >= 0 && state.current < state.tokens.size());
    return state.tokens.at(state.current);
}

[[nodiscard]] const lox::token& previous(const parser_state& state) LOX_NOEXCEPT
{
    assert(state.current > 0);
    return state.tokens.at(state.current - 1);
}

[[maybe_unused]] const lox::token& advance(parser_state& state) LOX_NOEXCEPT
{
    if (!is_at_end(state)) {
        state.current++;
    }

    assert(state.current <= state.tokens.size());
    return previous(state);
}

[[nodiscard]] bool check(const parser_state& state,
  token_type type) LOX_NOEXCEPT
{
    if (is_at_end(state)) {
        return false;
    }

    return peek(state).type == type;
}

void log_error(const lox::token& token,
  std::string_view message,
  bool raise_exception = false)
{
    lox::log_error(token.line_str, token.line, token.column_end, message);
    if (raise_exception) {
        throw parse_error{};
    }
}

#if 0
void synchronize(parser_state& state) LOX_NOEXCEPT
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

    advance(state);
    while (!is_at_end(state)) {
        if (previous(state).type == token_type::SEMICOLON) {
            return;
        }

        if (std::find(statements.cbegin(),
              statements.cend(),
              peek(state).type) != statements.cend()) {
            return;
        }
    }
}
#endif

[[maybe_unused]] const lox::token& consume(parser_state& state,
  token_type type,
  std::string_view error_message)
{
    if (!check(state, type)) {
        log_error(peek(state), error_message);
    }

    return advance(state);
}

[[nodiscard]] bool match(parser_state& state,
  std::initializer_list<token_type> tokens) LOX_NOEXCEPT
{
    for (const auto type : tokens) {
        if (check(state, type)) {
            advance(state);
            return true;
        }
    }

    return false;
}

template<typename T, typename... Args>
[[nodiscard]] lox::copyable<lox::expr*> make_copyable(
  Args&&... args) LOX_NOEXCEPT
{
    return lox::copyable<lox::expr*>{ std::forward<Args>(args)... };
}

[[nodiscard]] lox::expr parse_primary(parser_state& state) LOX_NOEXCEPT
{
    if (match(state, { token_type::NIL })) {
        return lox::literal{ nullptr };
    }

    if (match(state, { token_type::FALSE })) {
        return lox::literal{ lox::object{ false } };
    }

    if (match(state, { token_type::TRUE })) {
        return lox::literal{ lox::object{ true } };
    }

    if (match(state, { token_type::NUMBER, token_type::STRING })) {
        return lox::literal{ previous(state).literal };
    }

    if (match(state, { token_type::IDENTIFIER })) {
        return lox::variable{ previous(state) };
    }

    if (match(state, { token_type::LEFT_PAREN })) {
        auto expression = parse_ternary(state);
        try {
            consume(state,
              token_type::RIGHT_PAREN,
              "Expected ')' after expression '('.");
        }
        catch (parse_error&) {
        }

        return lox::grouping{ expr_c{ std::move(expression) } };
    }

    if (state.current > 0 && previous(state).type == token_type::EQUAL_EQUAL) {
        log_error(previous(state), "Unterminated comparison.");
    }

    return {};
}

[[nodiscard]] lox::expr parse_unary(parser_state& state) LOX_NOEXCEPT
{
    if (match(state, { token_type::BANG, token_type::MINUS })) {
        const lox::token& opr = previous(state);
        auto right = parse_unary(state);
        return lox::unary{ opr, expr_c{ std::move(right) } };
    }

    return parse_primary(state);
}

[[nodiscard]] lox::expr parse_factor(parser_state& state) LOX_NOEXCEPT
{
    auto expression = parse_unary(state);
    while (match(state, { token_type::SLASH, token_type::STAR })) {
        const lox::token& opr = previous(state);
        auto right = parse_unary(state);
        expression = lox::binary{
            expr_c{ std::move(expression) }, opr, expr_c{ std::move(right) }
        };
    }

    return expression;
}

[[nodiscard]] lox::expr parse_term(parser_state& state) LOX_NOEXCEPT
{
    auto expression = parse_factor(state);
    while (match(state, { token_type::MINUS, token_type::PLUS })) {
        const lox::token& opr = previous(state);
        auto right = parse_factor(state);
        expression = lox::binary{
            expr_c{ std::move(expression) }, opr, expr_c{ std::move(right) }
        };
    }

    return expression;
}

[[nodiscard]] lox::expr parse_comparison(parser_state& state) LOX_NOEXCEPT
{
    auto expression = parse_term(state);

    static const std::initializer_list<token_type> types{ token_type::GREATER,
        token_type::GREATER_EQUAL,
        token_type::LESS,
        token_type::LESS_EQUAL };

    while (match(state, types)) {
        const lox::token& opr = previous(state);
        auto right = parse_term(state);
        expression = lox::binary{
            expr_c{ std::move(expression) }, opr, expr_c{ std::move(right) }
        };
    }

    return expression;
}

[[nodiscard]] lox::expr parse_equality(parser_state& state) LOX_NOEXCEPT
{
    auto expression = parse_comparison(state);
    while (match(state, { token_type::BANG_EQUAL, token_type::EQUAL_EQUAL })) {
        const lox::token& opr = previous(state);
        lox::expr right = parse_comparison(state);
        expression = lox::binary{
            expr_c{ std::move(expression) }, opr, expr_c{ std::move(right) }
        };
    }

    return expression;
}

[[nodiscard]] lox::expr parse_assignment(parser_state& state) LOX_NOEXCEPT
{
    auto expression = parse_equality(state);
    if (!match(state, { token_type::EQUAL })) {
        return expression;
    }

    const lox::token& equals = previous(state);
    lox::expr value = parse_assignment(state);
    if (std::holds_alternative<lox::variable>(expression)) {
        lox::token name = std::get<lox::variable>(expression).name;
        expression =
          lox::assignment{ std::move(name), expr_c{ std::move(value) } };
    }
    else {
        log_error(equals, "Invalid assignment target.", false);
    }

    return expression;
}

[[nodiscard]] lox::expr parse_expression(parser_state& state) LOX_NOEXCEPT
{
    return parse_assignment(state);
}

[[nodiscard]] lox::expr parse_ternary(parser_state& state) LOX_NOEXCEPT
{
    std::vector<lox::expr> exprs;
    try {
        exprs.push_back(parse_expression(state));
    }
    catch (parse_error&) {
        advance(state);
        exprs.push_back({});
    }

    const auto push = [&exprs](auto expr) {
        if (std::holds_alternative<lox::ternary>(expr)) {
            exprs.push_back(lox::grouping{ expr_c{ std::move(expr) } });
        }
        else {
            exprs.push_back(std::move(expr));
        }
    };

    if (match(state, { token_type::QUESTION_MARK })) {
        push(parse_ternary(state));

        if (match(state, { token_type::COLON })) {
            push(parse_ternary(state));
        }
    }

    if (exprs.size() == 1) {
        return std::move(exprs.front());
    }

    if (exprs.size() != 3) {
        log_error(peek(state), "Expected ':' to finish the ternary operator.");

        return lox::ternary{ expr_c{ std::move(exprs[0]) },
            expr_c{ std::move(exprs[1]) },
            expr_c{} };
    }

    assert(exprs.size() == 3);
    return lox::ternary{ expr_c{ std::move(exprs[0]) },
        expr_c{ std::move(exprs[1]) },
        expr_c{ std::move(exprs[2]) } };
}

lox::stmt parse_var(parser_state& state)
{
    lox::token name =
      consume(state, token_type::IDENTIFIER, "Expected a variable name.");

    lox::expr initializer{};
    if (match(state, { token_type::EQUAL })) {
        initializer = parse_expression(state);
    }

    consume(state, token_type::SEMICOLON, "Expected ';' after value.");
    return lox::var_stmt{ std::move(name),
        std::holds_alternative<std::monostate>(initializer)
          ? expr_c{}
          : expr_c{ std::move(initializer) } };
}

lox::stmt parse_expr_statement(parser_state& state)
{
    lox::expr expr = parse_ternary(state);
    consume(state, token_type::SEMICOLON, "Expected ';' after value.");
    return lox::expr_stmt{ expr_c{ std::move(expr) } };
}

lox::stmt parse_print(parser_state& state)
{
    lox::expr expr = parse_ternary(state);
    consume(state, token_type::SEMICOLON, "Expected ';' after value.");
    return lox::print_stmt{ expr_c{ std::move(expr) } };
}

lox::stmt parse_stmt(parser_state& state)
{
    if (match(state, { token_type::PRINT })) {
        return parse_print(state);
    }

    return parse_expr_statement(state);
}

lox::stmt parse_declaration(parser_state& state)
{
    if (match(state, { token_type::VAR })) {
        return parse_var(state);
    }

    return parse_stmt(state);
}
}

std::vector<lox::stmt> lox::parse(
  const std::vector<lox::token>& tokens) LOX_NOEXCEPT
{
    assert(!tokens.empty());

    std::vector<lox::stmt> statements{};
    parser_state state{ tokens, 0 };
    while (peek(state).type != token_type::END_OF_FILE && !is_at_end(state)) {
        statements.push_back(parse_declaration(state));
    }

    return statements;
}
