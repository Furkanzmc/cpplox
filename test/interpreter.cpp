#include "interpreter.h"
#include "environment.h"

#include "parser.h"
#include "scanner.h"

#include "exceptions.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Test Interpreter", "[lox++::interpreter")
{
    GIVEN("Test simple math")
    {
        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 2 + 3;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const lox::object result = lox::interpret(statements.front(), env);
            CHECK(std::get<double>(result) == 5);
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 - 1;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const lox::object result = lox::interpret(statements.front(), env);
            CHECK(std::get<double>(result) == 2);
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 / 1;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const lox::object result = lox::interpret(statements.front(), env);
            CHECK(std::get<double>(result) == 3);
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 / 0;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            CHECK_THROWS_AS(
              lox::interpret(statements.front(), env), lox::runtime_error);
        }

        {
            const auto statements =
              lox::parse(lox::scan_tokens("var sum = 3 * 2;").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const lox::object result = lox::interpret(statements.front(), env);
            CHECK(std::get<double>(result) == 6);
        }
    }

    GIVEN("Test print")
    {
        {
            const auto statements =
              lox::parse(lox::scan_tokens("print (2 + 3);").tokens);
            CHECK(statements.size() == 1);

            lox::environment env{};
            const lox::object result = lox::interpret(statements.front(), env);
            CHECK(std::get<std::string>(result) == "5");
        }
    }

    GIVEN("Test variable declaration and assignment")
    {
        const auto statements = lox::parse(
          lox::scan_tokens("var sum = 2 + 3; sum = 32; print sum;").tokens);
        CHECK(statements.size() == 3);

        lox::environment env{};
        {
            const lox::object result = lox::interpret(statements.at(0), env);
            CHECK(std::get<double>(result) == 5);
        }
        {
            const lox::object result = lox::interpret(statements.at(1), env);
            CHECK(std::get<double>(result) == 32);
        }
        {
            const lox::object result = lox::interpret(statements.at(2), env);
            CHECK(std::get<std::string>(result) == "32");
        }
    }
}
