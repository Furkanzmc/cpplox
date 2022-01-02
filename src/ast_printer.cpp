#include "ast_printer.h"

#include <sstream>
#include <variant>
#include <string_view>

namespace {
template<typename>
[[maybe_unused]] inline constexpr bool always_false_v = false;

template<typename B, typename T>
inline constexpr bool is_same_v = std::is_same_v<B, std::unique_ptr<T>>;

template<typename... Args>
[[nodiscard]] std::string parenthesize(
  std::variant<std::string_view, double> name,
  Args&... args) LOX_NOEXCEPT;

inline constexpr auto object_visitor = [](std::stringstream& ss, auto&& arg) {
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
    else {
        static_assert(always_false_v<T>, "non-exhaustive expr_visitor!");
    }
};

inline constexpr auto expr_visitor = [](std::stringstream& ss, auto&& arg) {
    using T = std::decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, binary>) {
        ss << parenthesize(arg->oprtor.lexeme, arg);
    }
    else if constexpr (is_same_v<T, grouping>) {
        ss << parenthesize("group", arg);
    }
    else if constexpr (is_same_v<T, literal>) {
        std::visit(
          [&ss](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              object_visitor(ss, std::forward<const T>(arg));
          },
          arg->value);
    }
    else if constexpr (is_same_v<T, unary>) {
        ss << parenthesize(arg->oprtor.lexeme, arg);
    }
    else {
        static_assert(always_false_v<T>, "non-exhaustive expr_visitor!");
    }
};

void parenthesize(std::stringstream& ss,
  const std::unique_ptr<binary>& bin) LOX_NOEXCEPT
{
    ss << print_ast(bin->left);
    ss << ' ';
    ss << print_ast(bin->right);
}

void parenthesize(std::stringstream& ss,
  const std::unique_ptr<grouping>& group) LOX_NOEXCEPT
{
    ss << print_ast(group->expression);
}

void parenthesize(std::stringstream& ss,
  const std::unique_ptr<unary>& un) LOX_NOEXCEPT
{
    ss << print_ast(un->right);
}

template<typename... Args>
[[nodiscard]] std::string parenthesize(
  std::variant<std::string_view, double> name,
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
}

std::string print_ast(const expr& ex) LOX_NOEXCEPT
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
