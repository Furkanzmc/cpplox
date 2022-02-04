#include "ast_printer.h"

#include "defs.h"

#include <sstream>
#include <variant>
#include <string_view>

namespace {
template<typename>
[[maybe_unused]] constexpr bool always_false_v = false;

template<typename... Args>
[[nodiscard]] std::string parenthesize(
  std::variant<std::string_view, double> name,
  Args&... args) LOX_NOEXCEPT;

std::string print_ast(const lox::expr& ex) LOX_NOEXCEPT;

constexpr auto object_visitor = [](std::stringstream& ss, auto&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, std::string_view>) {
        ss << arg;
    }
    else if constexpr (std::is_same_v<T, double>) {
        ss << arg;
    }
    else if constexpr (std::is_same_v<T, bool>) {
        ss << std::boolalpha << arg;
    }
    else if constexpr (std::is_same_v<T, std::nullptr_t>) {
        ss << "null";
    }
    else if constexpr (std::is_same_v<T, std::monostate>) {
        ss << "UNKNOWN";
    }
    else {
        static_assert(always_false_v<T>, "non-exhaustive object_visitor!");
    }
};

constexpr auto expr_visitor = [](std::stringstream& ss, auto&& arg) {
    using T = std::decay_t<decltype(arg)>;

    if constexpr (std::is_same_v<T, std::monostate>) {
        ss << "UNKNOWN";
    }
    else if constexpr (std::is_same_v<T, lox::binary>) {
        ss << parenthesize(arg.oprtor.lexeme, arg);
    }
    else if constexpr (std::is_same_v<T, lox::grouping>) {
        ss << parenthesize("group", arg);
    }
    else if constexpr (std::is_same_v<T, lox::literal>) {
        std::visit(
          [&ss](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              object_visitor(ss, std::forward<const T>(arg));
          },
          arg.value);
    }
    else if constexpr (std::is_same_v<T, lox::unary>) {
        ss << parenthesize(arg.oprtor.lexeme, arg);
    }
    else if constexpr (std::is_same_v<T, lox::ternary>) {
        ss << *arg.first << " ? " << *arg.second << " : " << *arg.third;
    }
    else {
        static_assert(always_false_v<T>, "non-exhaustive expr_visitor!");
    }
};

void parenthesize(std::stringstream& ss, const lox::binary& bin) LOX_NOEXCEPT
{
    ss << print_ast(*bin.left);
    ss << ' ';
    ss << print_ast(*bin.right);
}

void parenthesize(std::stringstream& ss,
  const lox::grouping& group) LOX_NOEXCEPT
{
    ss << print_ast(*group.expression);
}

void parenthesize(std::stringstream& ss, const lox::unary& un) LOX_NOEXCEPT
{
    ss << print_ast(*un.right);
}

template<typename... Args>
std::string parenthesize(std::variant<std::string_view, double> name,
  Args&... args) LOX_NOEXCEPT
{
    std::stringstream ss;
    ss << '(';

    std::visit(
      [&ss](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          object_visitor(ss, std::forward<const T>(arg));
      },
      name);

    ss << " ";
    parenthesize(ss, args...);
    ss << ')';

    return ss.str();
}

std::string print_ast(const lox::expr& ex) LOX_NOEXCEPT
{
    std::stringstream ss;
    std::visit(
      [&ss](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;

          expr_visitor(ss, std::forward<const T>(arg));
      },
      ex);

    return ss.str();
}
}

std::ostream& operator<<(std::ostream& os, const lox::expr& expr)
{
    assert(!expr.valueless_by_exception());
    os << print_ast(expr);
    return os;
}
