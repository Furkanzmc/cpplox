#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

SCENARIO("A simple print statement.", "[lox++::scanner")
{
    GIVEN("A print statement with string.")
    {
        scanner scn = scan_tokens("print \"Hello, World!\"");

        const auto& tokens = scn.tokens;
        THEN("There's a print IDENTIFIER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::IDENTIFIER;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(std::get<std::string_view>(foundIt->lexeme) == "print");
            CHECK(foundIt->column_start == 0);
            CHECK(foundIt->column_end == 5);
        }

        THEN("There's a STRING.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::STRING;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(
              std::get<std::string_view>(foundIt->lexeme) == "Hello, World!");
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 21);
        }
    }

    GIVEN("A print statement with non-fractional number.")
    {
        scanner scn = scan_tokens("print 1234");

        const auto& tokens = scn.tokens;
        THEN("There's a NUMBER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::NUMBER;
              });

            CHECK(foundIt != tokens.cend());
            REQUIRE(std::holds_alternative<double>(foundIt->lexeme));
            CHECK(std::get<double>(foundIt->lexeme) == 1234);
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 10);
        }
    }

    GIVEN("A print statement with fractional number.")
    {
        scanner scn = scan_tokens("print 1234.3");

        const auto& tokens = scn.tokens;
        THEN("There's a fractional NUMBER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::NUMBER;
              });

            CHECK(foundIt != tokens.cend());
            REQUIRE(std::holds_alternative<double>(foundIt->lexeme));
            CHECK(std::get<double>(foundIt->lexeme) == 1234.3);
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 10);
        }
    }
}
