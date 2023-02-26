#include "parser.h"
#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace lox;

SCENARIO("Test AST", "[lox++::parser")
{
    GIVEN("Test semicolon requirement.")
    {
        const auto statements = parse(scan_tokens("print 32"));
        CHECK(statements.size() == 1);
    }

    GIVEN("Simple ternary operator.")
    {
        const auto statements = parse(scan_tokens("1 ? 2 : 3;"));
        CHECK(statements.size() == 1);

        const auto stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        const auto expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::ternary>(*expr_stmt.expression));
    }

    GIVEN("Simple equality.")
    {
        auto statements = parse(scan_tokens("1 == 2;"));
        CHECK(statements.size() == 1);

        auto stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        auto expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::EQUAL_EQUAL);

        statements = parse(scan_tokens("1 != 2;"));

        stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::BANG_EQUAL);
    }

    GIVEN("Simple less than comparison.")
    {
        auto statements = parse(scan_tokens("1 < 2;"));
        CHECK(statements.size() == 1);

        auto stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        auto expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::LESS);

        statements = parse(scan_tokens("1 <= 2;"));
        CHECK(statements.size() == 1);

        stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::LESS_EQUAL);
    }

    GIVEN("Simple greater than comparison.")
    {
        auto statements = parse(scan_tokens("1 > 2;"));
        CHECK(statements.size() == 1);

        auto stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        auto expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::GREATER);

        statements = parse(scan_tokens("1 >= 2;"));
        CHECK(statements.size() == 1);

        stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::expr_stmt>(stmt));
        expr_stmt = std::get<lox::expr_stmt>(stmt);
        CHECK(std::holds_alternative<lox::binary>(*expr_stmt.expression));

        CHECK(std::get<lox::binary>(*expr_stmt.expression).oprtor.type ==
              token::token_type::GREATER_EQUAL);
    }

    GIVEN("Print statement.")
    {
        auto statements = parse(scan_tokens("var a = 0; print a;"));
        CHECK(statements.size() == 2);

        const auto var_stmt = statements.at(0);
        CHECK(std::holds_alternative<lox::var_stmt>(var_stmt));

        const auto print_stmt = statements.at(1);
        CHECK(std::holds_alternative<lox::print_stmt>(print_stmt));
        const auto print = std::get<lox::print_stmt>(print_stmt);
        CHECK(std::holds_alternative<lox::variable>(*print.expression));

        const auto variable = std::get<lox::variable>(*print.expression);
        CHECK(variable.name.lexeme == "a");
    }
}
