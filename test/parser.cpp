#include "parser.h"
#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace lox;

SCENARIO("Test AST", "[lox++::parser")
{
    GIVEN("Simple ternary operator.")
    {
        const auto expr = parse(scan_tokens("1 ? 2 : 3"));

        CHECK(std::holds_alternative<lox::ternary>(expr));
    }

    GIVEN("Simple equality.")
    {
        auto expr = parse(scan_tokens("1 == 2"));

        CHECK(std::holds_alternative<lox::binary>(expr));
        CHECK(std::get<lox::binary>(expr).oprtor.type ==
              token::token_type::EQUAL_EQUAL);

        expr = parse(scan_tokens("1 != 2"));

        CHECK(std::get<lox::binary>(expr).oprtor.type ==
              token::token_type::BANG_EQUAL);
    }

    GIVEN("Simple comparison.")
    {
        auto expr = parse(scan_tokens("1 < 2"));
        CHECK(std::holds_alternative<lox::binary>(expr));
        CHECK(
          std::get<lox::binary>(expr).oprtor.type == token::token_type::LESS);

        expr = parse(scan_tokens("1 <= 2"));
        CHECK(std::holds_alternative<lox::binary>(expr));
        CHECK(std::get<lox::binary>(expr).oprtor.type ==
              token::token_type::LESS_EQUAL);

        expr = parse(scan_tokens("1 > 2"));
        CHECK(std::holds_alternative<lox::binary>(expr));
        CHECK(std::get<lox::binary>(expr).oprtor.type ==
              token::token_type::GREATER);

        expr = parse(scan_tokens("1 >= 2"));
        CHECK(std::holds_alternative<lox::binary>(expr));
        CHECK(std::get<lox::binary>(expr).oprtor.type ==
              token::token_type::GREATER_EQUAL);
    }
}
