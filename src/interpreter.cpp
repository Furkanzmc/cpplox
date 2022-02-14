#include "interpreter.h"

#include "utils.h"
#include "exceptions.h"

#include <memory>
#include <optional>
#include <sstream>

#ifndef LOX_EXCEPTION_ENABLED
#error "Interpreter relies on exceptions to be enabled."
#endif

namespace {
using token_type = lox::token::token_type;

template<typename>
[[maybe_unused]] constexpr bool always_false_v = false;

// Raises lox::runtime_error if there's an error.
void check_number_operand(const lox::token& token,
  const lox::object& left,
  std::optional<std::reference_wrapper<const lox::object>> right = {})
{
    if (right.has_value() && std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right->get())) {
        return;
    }

    if (!right.has_value() && std::holds_alternative<double>(left)) {
        return;
    }

    constexpr const char* msg{ "Operand must be a number." };
    lox::log_error(token.line_str, token.line, token.column_end, msg);
    throw lox::runtime_error{ token, msg };
}

// Raises lox::runtime_error if there's an error.
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

[[nodiscard]] lox::object interpret_binary(const lox::binary& binary)
{
    const auto left = lox::interpret(*binary.left);
    const auto right = lox::interpret(*binary.right);

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

[[nodiscard]] lox::object interpret_unary(const lox::unary& expr)
{
    const auto right = lox::interpret(*expr.right);
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

constexpr auto interpreter_visitor = [](auto&& arg) -> lox::object {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, lox::literal>) {
        return arg.value;
    }
    else if constexpr (std::is_same_v<T, lox::grouping>) {
        return lox::interpret(*arg.expression);
    }
    else if constexpr (std::is_same_v<T, lox::unary>) {
        return interpret_unary(arg);
    }
    else if constexpr (std::is_same_v<T, lox::binary>) {
        return interpret_binary(arg);
    }
    else if constexpr (std::is_same_v<T, lox::ternary>) {
        return {};
    }
    else if constexpr (std::is_same_v<T, std::monostate>) {
        return {};
    }
    else {
        static_assert(always_false_v<T>, "Unhandled type.");
    }

    return {};
};
}

lox::object lox::interpret(const expr& expression)
{
    return std::visit(interpreter_visitor, expression);
}
