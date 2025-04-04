#include "scanner.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using namespace lox;

SCENARIO("Test all the token types.", "[lox++::scanner]")
{
    GIVEN("A print statement with string.")
    {
        const auto tokens = scan_tokens("print \"Hello, World!\"").tokens;
        THEN("There's a print IDENTIFIER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::PRINT;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(foundIt->lexeme == "print");
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
            CHECK(foundIt->lexeme == "\"Hello, World!\"");
            REQUIRE(std::holds_alternative<std::string_view>(foundIt->literal));
            CHECK(
              std::get<std::string_view>(foundIt->literal) == "Hello, World!");
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 21);
        }
    }

    GIVEN("A print statement with non-fractional number.")
    {
        const auto tokens = scan_tokens("print 1234").tokens;
        THEN("There's a NUMBER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::NUMBER;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(foundIt->lexeme == "1234");
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 10);
        }
    }

    GIVEN("A string with an unterminated quotation mark.")
    {
        using error_type = lox::scan_result::error::error_type;
        const auto result = scan_tokens("print \"Hello, World!");
        THEN("There's an error.")
        {
            CHECK(!result.errors.empty());
            CHECK(
              result.errors.front().type == error_type::UNTERMINATED_STRING);
            CHECK(result.errors.front().line == 0);
        }
    }

    GIVEN("A print statement with fractional number.")
    {
        const auto tokens = scan_tokens("print 1234.3").tokens;
        THEN("There's a fractional NUMBER.")
        {
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::NUMBER;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(foundIt->lexeme == "1234.3");
            CHECK(foundIt->column_start == 6);
            CHECK(foundIt->column_end == 12);
        }
    }

    GIVEN("A comment")
    {
        THEN("Single line comment is parsed.")
        {
            const auto tokens = scan_tokens("//Hello there").tokens;
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::COMMENT;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(foundIt->lexeme == "Hello there");
            CHECK(foundIt->column_start == 0);
            CHECK(foundIt->column_end == 13);
        }

        THEN("Multi line comment is parsed.")
        {
            const auto tokens = scan_tokens("/*Hello there*/").tokens;
            auto foundIt =
              std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
                  return tkn.type == token::token_type::COMMENT;
              });

            CHECK(foundIt != tokens.cend());
            CHECK(foundIt->lexeme == "Hello there");
            CHECK(foundIt->column_start == 0);
            CHECK(foundIt->column_end == 15);
        }
    }

    GIVEN("A function.")
    {
        const auto tokens =
          scan_tokens("fun add(a, b) { return a + b }").tokens;
        auto foundIt = std::find_if(tokens.cbegin(),
          tokens.cend(),
          [](const auto& tkn) { return tkn.type == token::token_type::FUN; });

        CHECK(foundIt != tokens.cend());
        CHECK(foundIt->lexeme == "fun");
        CHECK(foundIt->column_start == 0);
        CHECK(foundIt->column_end == 3);
    }

    GIVEN("A class declaration.")
    {
        const auto tokens =
          scan_tokens("class Breakfast {"
                      "cook() {"
                      "print \"Eggs a-fryin'!\";"
                      "}"

                      "serve(who) {"
                      "print \"Enjoy your breakfast, \" + who + \".\";"
                      "}"
                      "}")
            .tokens;
        auto foundIt = std::find_if(tokens.cbegin(),
          tokens.cend(),
          [](const auto& tkn) { return tkn.type == token::token_type::CLASS; });

        CHECK(foundIt != tokens.cend());
        CHECK(foundIt->lexeme == "class");
        CHECK(foundIt->column_start == 0);
        CHECK(foundIt->column_end == 5);
    }

    GIVEN("A ternary operator.")
    {
        const auto tokens = scan_tokens("true ? false : true").tokens;
        auto foundIt =
          std::find_if(tokens.cbegin(), tokens.cend(), [](const auto& tkn) {
              return tkn.type == token::token_type::QUESTION_MARK;
          });

        CHECK(foundIt != tokens.cend());
        CHECK(foundIt->lexeme == "?");
        CHECK(foundIt->column_start == 5);
        CHECK(foundIt->column_end == 6);
        CHECK(std::distance(tokens.begin(), foundIt) == 1);

        foundIt = std::find_if(tokens.cbegin(),
          tokens.cend(),
          [](const auto& tkn) { return tkn.type == token::token_type::COLON; });

        CHECK(foundIt != tokens.cend());
        CHECK(foundIt->lexeme == ":");
        CHECK(foundIt->column_start == 13);
        CHECK(foundIt->column_end == 14);
        CHECK(std::distance(tokens.begin(), foundIt) == 3);
    }

    GIVEN("A print statement with a number and semicolon.")
    {
        const auto tokens = scan_tokens("print 32;").tokens;

        auto foundIt = tokens.begin();
        CHECK(foundIt->type == token::token_type::PRINT);
        CHECK(foundIt->lexeme == "print");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::NUMBER);
        CHECK(foundIt->lexeme == "32");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::SEMICOLON);
        CHECK(foundIt->lexeme == ";");
    }

    GIVEN("A print statement with a string and semicolon.")
    {
        const auto tokens = scan_tokens("print \"Hello world!\";").tokens;

        auto foundIt = tokens.begin();
        CHECK(foundIt->type == token::token_type::PRINT);
        CHECK(foundIt->lexeme == "print");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::STRING);
        CHECK(foundIt->lexeme == "\"Hello world!\"");
        CHECK(std::get<std::string_view>(foundIt->literal) == "Hello world!");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::SEMICOLON);
        CHECK(foundIt->lexeme == ";");
    }

    GIVEN("A print statement with an identifier and semicolon.")
    {
        const auto tokens = scan_tokens("print message;").tokens;

        auto foundIt = tokens.begin();
        CHECK(foundIt->type == token::token_type::PRINT);
        CHECK(foundIt->lexeme == "print");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::IDENTIFIER);
        CHECK(foundIt->lexeme == "message");

        foundIt = std::next(foundIt);
        CHECK(foundIt->type == token::token_type::SEMICOLON);
        CHECK(foundIt->lexeme == ";");
    }
}
