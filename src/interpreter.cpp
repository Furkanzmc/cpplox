#include "interpreter.h"

#include "ast_printer.h"
#include "exceptions.h"
#include "environment.h"
#include "utils.h"

#include <optional>
#include <sstream>
#include <cassert>

#ifndef LOX_EXCEPTION_ENABLED
#error "Interpreter relies on exceptions to be enabled."
#endif

namespace {
using token_type = lox::token::token_type;

template<typename>
[[maybe_unused]] constexpr bool always_false_v = false;

// Forward declerations

lox::object internal_interpret(const lox::expr& expression,
  lox::environment& env);
lox::object internal_interpret(const lox::stmt& statement,
  lox::environment& env);

// Raises lox::runtime_error if there's an error.

/*!
 * Checks if the left and right operands are numbers. If the right operand is
 * not provided, it checks if the left operand is a number.
 *
 * @internal
 * @param token The token that's being processed.
 * @param left The left operand.
 * @param right The right operand. Optional.
 * @throws lox::runtime_error If the operands are not numbers.
 */
void check_number_operand(const lox::token& token,
  const lox::object& left,
  std::optional<std::reference_wrapper<const lox::object>> right = {})
{
    if (right.has_value() && std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right->get())) {
        if (std::get<double>(right->get()) == 0) {
            constexpr const std::string_view msg{ "Division by zero." };
            lox::log_error(token.line_str, token.line, token.column_end, msg);
            throw lox::runtime_error{ token, msg.data() };
        }

        return;
    }

    if (!right.has_value() && std::holds_alternative<double>(left)) {
        return;
    }

    constexpr const std::string_view msg{ "Operand must be a number." };
    if (!std::holds_alternative<double>(left)) {
        lox::log_error(token.line_str, token.line, 0, msg);
        throw lox::runtime_error{ token, msg.data() };
    }

    lox::log_error(token.line_str, token.line, token.column_end, msg);
    throw lox::runtime_error{ token, msg.data() };
}

/*!
 * Checks if the left and right operands are strings or numbers. If the right
 * operand is not provided, it checks if the left operand is a string or a
 * number.
 *
 * @internal
 * @param token The token that's being processed.
 * @param left The left operand.
 * @param right The right operand. Optional.
 * @throws lox::runtime_error If the operands are not strings or numbers.
 */
void check_concatenation_types(const lox::token& token,
  const lox::object& left,
  std::optional<std::reference_wrapper<const lox::object>> right = {})
{
    const auto is_valid = [](const auto& obj) -> bool {
        return std::holds_alternative<std::string_view>(obj) ||
               std::holds_alternative<std::string>(obj) ||
               std::holds_alternative<double>(obj);
    };

    if (right.has_value() && is_valid(left) && is_valid(right->get())) {
        return;
    }

    if (!right.has_value() && is_valid(left)) {
        return;
    }

    constexpr const char* msg{ "Operand must be a string." };
    lox::log_error(token.line_str, token.line, token.column_end, msg);
    throw lox::runtime_error{ token, msg };
}

/*!
 * An object is considered truthy if it's a non-empty string, a non-zero number
 * or a boolean true.
 *
 * @internal
 * @param object The object to check.
 * @return True if the object is truthy, false otherwise.
 */
[[nodiscard]] bool is_truthy(const lox::object& object) LOX_NOEXCEPT
{
    if (std::holds_alternative<std::string_view>(object)) {
        return !std::get<std::string_view>(object).empty();
    }
    else if (std::holds_alternative<double>(object)) {
        return std::get<double>(object) > 0;
    }
    else if (std::holds_alternative<bool>(object)) {
        return std::get<bool>(object);
    }

    return false;
}

[[nodiscard]] lox::object interpret_binary(const lox::binary& binary,
  lox::environment& env)
{
    const auto left = internal_interpret(*binary.left, env);
    const auto right = internal_interpret(*binary.right, env);

    const auto type = binary.oprtor.type;
    if (type == token_type::MINUS) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) - std::get<double>(right);
    }
    else if (type == token_type::SLASH) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) / std::get<double>(right);
    }
    else if (type == token_type::STAR) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) * std::get<double>(right);
    }
    else if (type == token_type::PLUS) {
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return std::get<double>(left) + std::get<double>(right);
        }

        check_concatenation_types(binary.oprtor, left, right);

        const auto get_value = [](auto&& val) -> std::string {
            std::stringstream ss;
            if (std::holds_alternative<std::string_view>(val)) {
                ss << std::get<std::string_view>(val);
            }
            else if (std::holds_alternative<double>(val)) {
                ss << std::get<double>(val);
            }
            else {
                assert(std::holds_alternative<std::string>(val));
                ss << std::get<std::string>(val);
            }

            return ss.str();
        };

        return get_value(left) + get_value(right);
    }
    else if (type == token_type::GREATER) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) > std::get<double>(right);
    }
    else if (type == token_type::GREATER_EQUAL) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    }
    else if (type == token_type::LESS) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) < std::get<double>(right);
    }
    else if (type == token_type::LESS_EQUAL) {
        check_number_operand(binary.oprtor, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    }
    else if (type == token_type::EQUAL_EQUAL) {
        return left == right;
    }
    else if (type == token_type::BANG_EQUAL) {
        return left != right;
    }

    assert(false);
    return {};
}

[[nodiscard]] lox::object interpret_unary(const lox::unary& expr,
  lox::environment& env)
{
    const auto right = internal_interpret(*expr.right, env);
    const auto type = expr.oprtor.type;
    if (type == token_type::MINUS) {
        check_number_operand(expr.oprtor, right);
        return std::get<double>(right) * -1;
    }
    else if (type == token_type::BANG) {
        assert(std::holds_alternative<double>(right));
        return !is_truthy(right);
    }

    assert(false);
    return {};
}

[[nodiscard]] lox::object interpret_var_stmt(const lox::var_stmt& stmt,
  lox::environment& env)
{
    lox::object value{ nullptr };
    if (stmt.expression) {
        value = internal_interpret(*stmt.expression, env);
    }

    lox::env::define(env,
      std::string{ stmt.name.lexeme.data(), stmt.name.lexeme.size() },
      value);
    return value;
}

[[nodiscard]] lox::object interpret_assignment(const lox::assignment& expr,
  lox::environment& env)
{
    lox::object value{ internal_interpret(*expr.value, env) };

    lox::env::assign(env, expr.name, value);
    return value;
}

constexpr auto interpreter_visitor = [](auto&& arg,
                                       lox::environment& env) -> lox::object {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, lox::literal>) {
        return arg.value;
    }
    else if constexpr (std::is_same_v<T, lox::grouping>) {
        return internal_interpret(*arg.expression, env);
    }
    else if constexpr (std::is_same_v<T, lox::unary>) {
        return interpret_unary(arg, env);
    }
    else if constexpr (std::is_same_v<T, lox::binary>) {
        return interpret_binary(arg, env);
    }
    else if constexpr (std::is_same_v<T, lox::ternary>) {
        return {};
    }
    else if constexpr (std::is_same_v<T, lox::expr_stmt>) {
        return internal_interpret(*arg.expression, env);
    }
    else if constexpr (std::is_same_v<T, lox::print_stmt>) {
        std::stringstream ss;
        ss << internal_interpret(*arg.expression, env);
        std::string result{ ss.str() };
        std::cout << result;
        return result;
    }
    else if constexpr (std::is_same_v<T, lox::var_stmt>) {
        return interpret_var_stmt(arg, env);
    }
    else if constexpr (std::is_same_v<T, lox::variable>) {
        return lox::env::get(env, arg.name);
    }
    else if constexpr (std::is_same_v<T, lox::assignment>) {
        return interpret_assignment(arg, env);
    }
    else if constexpr (std::is_same_v<T, std::monostate>) {
        return {};
    }
    else {
        static_assert(always_false_v<T>, "Unhandled type.");
    }

    return {};
};

lox::object internal_interpret(const lox::expr& expression,
  lox::environment& env)
{
    return std::visit(
      [&env](auto&& arg) -> lox::object {
          return interpreter_visitor(std::forward<decltype(arg)>(arg), env);
      },
      expression);
}

lox::object internal_interpret(const lox::stmt& statement,
  lox::environment& env)
{
    return std::visit(
      [&env](auto&& arg) -> lox::object {
          return interpreter_visitor(std::forward<decltype(arg)>(arg), env);
      },
      statement);
}
}

lox::object lox::interpret(const stmt& statement, environment& env)
{
    return internal_interpret(statement, env);
}
