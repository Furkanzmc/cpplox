#include "interpreter.h"

#include <memory>
#include <optional>

template<typename T>
using expr_c = lox::copyable<lox::expr*>;

using token_type = lox::token::token_type;

template<typename>
[[maybe_unused]] constexpr bool always_false_v = false;

namespace {
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

[[nodiscard]] lox::object interpreter_binary(
  const lox::binary& binary) LOX_NOEXCEPT
{
    lox::object left = lox::interpret(*binary.left);
    lox::object right = lox::interpret(*binary.right);

    const auto type = binary.oprtor.type;
    if (type == token_type::MINUS) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));

        return std::get<double>(left) - std::get<double>(right);
    }
    else if (type == token_type::SLASH) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));

        return std::get<double>(left) / std::get<double>(right);
    }
    else if (type == token_type::STAR) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));

        return std::get<double>(left) * std::get<double>(right);
    }
    else if (type == token_type::PLUS) {
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return std::get<double>(left) + std::get<double>(right);
        }

        assert(std::holds_alternative<std::string_view>(left));
        assert(std::holds_alternative<std::string_view>(right));
        assert("Not implemented!" == nullptr);

        // return std::get<std::string_view>(left) *
        //        std::get<std::string_view>(right);
    }
    else if (type == token_type::GREATER) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));
        return std::get<double>(left) > std::get<double>(right);
    }
    else if (type == token_type::GREATER_EQUAL) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));
        return std::get<double>(left) >= std::get<double>(right);
    }
    else if (type == token_type::LESS) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));
        return std::get<double>(left) < std::get<double>(right);
    }
    else if (type == token_type::LESS_EQUAL) {
        assert(std::holds_alternative<double>(left));
        assert(std::holds_alternative<double>(right));
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

constexpr auto interpreter_visitor = [](auto&& arg) -> lox::object {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, lox::literal>) {
        return arg.value;
    }
    else if constexpr (std::is_same_v<T, lox::grouping>) {
        return lox::interpret(lox::expr{ arg });
    }
    else if constexpr (std::is_same_v<T, lox::unary>) {
        lox::object right = lox::interpret(lox::expr{ arg });
        const auto type = arg.oprtor.type;
        if (type == token_type::MINUS) {
            assert(std::holds_alternative<double>(right));
            return std::get<double>(right) * -1;
        }
        else if (type == token_type::BANG) {
            assert(std::holds_alternative<double>(right));
            return !is_truthy(right);
        }
    }
    else if constexpr (std::is_same_v<T, lox::binary>) {
        return interpreter_binary(arg);
    }

    return {};
};
}

lox::object lox::interpret(const expr& expression) LOX_NOEXCEPT
{
    return std::visit(interpreter_visitor, expression);
}
