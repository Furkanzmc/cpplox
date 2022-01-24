#include "parser.h"
#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace lox;

template<typename T>
using expr_h = std::unique_ptr<T>;

template<typename T>
constexpr auto handle = [](const auto& expr) -> const expr_h<T>& {
    return std::get<expr_h<T>>(expr);
};

SCENARIO("Test AST", "[lox++::parser")
{
    GIVEN("Simple ternary operator.")
    {
        const auto expr = parse(scan_tokens("1 ? 2 : 3"));

        CHECK(std::holds_alternative<expr_h<lox::ternary>>(expr));
    }

    GIVEN("Simple equality.")
    {
        auto expr = parse(scan_tokens("1 == 2"));

        CHECK(std::holds_alternative<expr_h<lox::binary>>(expr));
        CHECK(handle<lox::binary>(expr)->oprtor.type ==
              token::token_type::EQUAL_EQUAL);

        expr = parse(scan_tokens("1 != 2"));

        CHECK(handle<lox::binary>(expr)->oprtor.type ==
              token::token_type::BANG_EQUAL);
    }

    GIVEN("Simple comparison.")
    {
        auto expr = parse(scan_tokens("1 < 2"));
        CHECK(std::holds_alternative<expr_h<lox::binary>>(expr));
        CHECK(
          handle<lox::binary>(expr)->oprtor.type == token::token_type::LESS);

        expr = parse(scan_tokens("1 <= 2"));
        CHECK(std::holds_alternative<expr_h<lox::binary>>(expr));
        CHECK(handle<lox::binary>(expr)->oprtor.type ==
              token::token_type::LESS_EQUAL);

        expr = parse(scan_tokens("1 > 2"));
        CHECK(std::holds_alternative<expr_h<lox::binary>>(expr));
        CHECK(
          handle<lox::binary>(expr)->oprtor.type == token::token_type::GREATER);

        expr = parse(scan_tokens("1 >= 2"));
        CHECK(std::holds_alternative<expr_h<lox::binary>>(expr));
        CHECK(handle<lox::binary>(expr)->oprtor.type ==
              token::token_type::GREATER_EQUAL);
    }
}
