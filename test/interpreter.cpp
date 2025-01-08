#include "interpreter.h"
#include "environment.h"

#include "parser.h"
#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <optional>

SCENARIO("Test Interpreter", "[lox++::interpreter]")
{
    GIVEN("Test simple math")
    {
        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 2 + 3;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const std::optional<lox::object> result =
              lox::interpret(statements.front(), env)
                .and_then([](lox::object result) -> std::optional<lox::object> {
                    return std::move(result);
                })
                .or_else([](auto) -> std::optional<lox::object> { return {}; });
            REQUIRE(result.has_value());
            CHECK(std::get<double>(result.value()) == 5);
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 - 1;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            lox::interpret(statements.front(), env)
              .and_then(
                [](auto result) { CHECK(std::get<double>(result) == 2); })
              .or_else([](auto) { CHECK(false); });
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 / 1;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            lox::interpret(statements.front(), env)
              .and_then(
                [](auto result) { CHECK(std::get<double>(result) == 3); })
              .or_else([](auto) { CHECK(false); });
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 / 0;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            lox::interpret(statements.front(), env)
              .and_then([](auto) { CHECK(false); })
              .or_else([](auto) { CHECK(true); });
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 * 2;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            lox::interpret(statements.front(), env)
              .and_then(
                [](auto result) { CHECK(std::get<double>(result) == 6); })
              .or_else([](auto) { CHECK(false); });
        }
    }

    GIVEN("Test print")
    {
        {
            const auto statements =
              lox::parse(lox::scan_tokens("print (2 + 3);").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            lox::interpret(statements.front(), env)
              .and_then([](auto result) {
                  CHECK(std::get<std::string>(result) == "5");
              })
              .or_else([](auto) { CHECK(false); });
        }
    }

    GIVEN("Test variable declaration and assignment")
    {
        const auto statements = lox::parse(
          lox::scan_tokens("var sum = 2 + 3; sum = 32; print sum;").tokens);
        CHECK(statements.size() == 3);

        lox::environment env{};
        {
            lox::interpret(statements.at(0), env)
              .and_then(
                [](auto result) { CHECK(std::get<double>(result) == 5); })
              .or_else([](auto) { CHECK(false); });
        }
        {
            lox::interpret(statements.at(1), env)
              .and_then(
                [](auto result) { CHECK(std::get<double>(result) == 32); })
              .or_else([](auto) { CHECK(false); });
        }
        {
            lox::interpret(statements.at(2), env)
              .and_then([](auto result) {
                  CHECK(std::get<std::string>(result) == "32");
              })
              .or_else([](auto) { CHECK(false); });
        }
    }
}
