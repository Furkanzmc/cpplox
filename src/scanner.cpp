#include "scanner.h"

#include "utils.h"

#include <map>

using token_type = lox::token::token_type;

namespace {
struct scan_data {
    std::string_view source;
    std::vector<lox::token> tokens{};
    std::size_t start{ 0 };
    std::size_t line{ 0 };
    std::size_t current{ 0 };
    bool has_error{ false };
};

const std::map<std::string_view, token_type> s_keywords{
    { "and", token_type::AND },
    { "class", token_type::CLASS },
    { "else", token_type::ELSE },
    { "false", token_type::FALSE },
    { "for", token_type::FOR },
    { "fun", token_type::FUN },
    { "if", token_type::IF },
    { "nil", token_type::NIL },
    { "or", token_type::OR },
    { "print", token_type::PRINT },
    { "super", token_type::SUPER },
    { "this", token_type::THIS },
    { "true", token_type::TRUE },
    { "var", token_type::VAR },
    { "while", token_type::WHILE },
};

std::string_view get_current_line_str(const scan_data& scn) LOX_NOEXCEPT
{
    std::size_t index{ scn.start };
    while (index > 0 && scn.source.at(index) != '\n') {
        index--;
    }

    std::size_t start_index{ index };
    index = scn.current;
    while (index < scn.source.size() && scn.source.at(index) != '\n') {
        index++;
    }

    std::size_t end_index{ index };
    return scn.source.substr(start_index, end_index);
}

void log_error(const scan_data& scn, std::string_view message) LOX_NOEXCEPT
{
    lox::log_error(get_current_line_str(scn), scn.line + 1, scn.start, message);
}

[[nodiscard]] bool is_at_end(const scan_data& scn) LOX_NOEXCEPT
{
    return scn.current >= scn.source.size();
}

char advance(scan_data& scn, int count = 1) LOX_NOEXCEPT
{
    char ch = scn.source.at(scn.current);
    scn.current += count;
    return ch;
}

[[nodiscard]] char peek(const scan_data& scn) LOX_NOEXCEPT
{
    if (is_at_end(scn)) {
        return '\0';
    }

    return scn.source.at(scn.current);
}

[[nodiscard]] char peek_next(const scan_data& scn) LOX_NOEXCEPT
{
    if (scn.current + 1 >= scn.source.size()) {
        return '\0';
    }

    return scn.source.at(scn.current + 1);
}

[[nodiscard]] bool match(scan_data& scn, char expected) LOX_NOEXCEPT
{
    if (is_at_end(scn)) {
        return false;
    }

    if (scn.source.at(scn.current) != expected) {
        return false;
    }

    scn.current++;
    return true;
}

[[nodiscard]] lox::token create_token(const scan_data& scn,
  token_type type,
  lox::object literal) LOX_NOEXCEPT
{
    return { type,
        scn.source.substr(scn.start, scn.current - scn.start),
        literal,
        scn.line,
        scn.start,
        scn.current,
        get_current_line_str(scn) };
}

void scan_string(scan_data& scn) LOX_NOEXCEPT
{
    while (peek(scn) != '"' && !is_at_end(scn)) {
        if (peek(scn) == '\n') {
            scn.line++;
        }

        advance(scn);
    }

    if (is_at_end(scn)) {
        scn.has_error = true;
        log_error(scn, "Unterminated string.");
        return;
    }

    // Closing '"'
    advance(scn);

    scn.tokens.push_back(lox::token{ token_type::STRING,
      scn.source.substr(scn.start, scn.current - scn.start),
      scn.source.substr(scn.start + 1, scn.current - scn.start - 2),
      scn.line,
      scn.start,
      scn.current,
      get_current_line_str(scn) });
}

void scan_number(scan_data& scn) LOX_NOEXCEPT
{
    // First character was consumed before calling scan_number.
    while (std::isdigit(peek(scn)) ||
           (peek(scn) == '.' && std::isdigit(peek_next(scn)))) {
        advance(scn);
    }

    const auto num_str = scn.source.substr(scn.start, scn.current - scn.start);
    scn.tokens.push_back(lox::token{ token_type::NUMBER,
      num_str,
      std::stod(std::string{ num_str }),
      scn.line,
      scn.start,
      scn.current,
      get_current_line_str(scn) });
}

void scan_comment(scan_data& scn) LOX_NOEXCEPT
{
    bool is_multi_line{ false };
    while (true) {
        char ch = peek(scn);
        if (!is_multi_line && (ch == '\n' || is_at_end(scn))) {
            break;
        }
        else if (!is_multi_line && ch == '*') {
            is_multi_line = true;
        }
        else if (is_multi_line && ch == '*' && peek_next(scn) == '/') {
            // Advance for the comment ending `*/`
            advance(scn, 2);
            break;
        }
        else if (is_at_end(scn)) {
            break;
        }

        if (ch == '\n') {
            scn.line++;
        }

        advance(scn);
    }

    scn.tokens.push_back(lox::token{ token_type::COMMENT,
      // +2 for the begining `//` or `/*`.
      scn.source.substr(
        scn.start + 2, (scn.current - scn.start - 2) - (is_multi_line ? 2 : 0)),
      nullptr,
      scn.line,
      scn.start,
      scn.current,
      get_current_line_str(scn) });
}

void scan_identifier(scan_data& scn) LOX_NOEXCEPT
{
    while (lox::isalnum(peek(scn))) {
        advance(scn);
    }

    std::string_view text{ scn.source.substr(
      scn.start, scn.current - scn.start) };
    const auto foundIt = s_keywords.find(text);
    if (foundIt == s_keywords.cend()) {
        if (foundIt->second == token_type::TRUE) {
            scn.tokens.push_back(
              create_token(scn, token_type::IDENTIFIER, true));
        }
        else if (foundIt->second == token_type::FALSE) {
            scn.tokens.push_back(
              create_token(scn, token_type::IDENTIFIER, false));
        }
        else if (foundIt->second == token_type::NIL) {
            scn.tokens.push_back(
              create_token(scn, token_type::IDENTIFIER, nullptr));
        }
        else {
            scn.tokens.push_back(create_token(scn, token_type::IDENTIFIER, {}));
        }
    }
    else {
        scn.tokens.push_back(create_token(scn, foundIt->second, {}));
    }
}

void scan_tokens_impl(scan_data& scn) LOX_NOEXCEPT
{
    const char ch = advance(scn);
    switch (ch) {
        case '(':
            scn.tokens.push_back(create_token(scn, token_type::LEFT_PAREN, {}));
            break;
        case ')':
            scn.tokens.push_back(
              create_token(scn, token_type::RIGHT_PAREN, {}));
            break;
        case '{':
            scn.tokens.push_back(create_token(scn, token_type::LEFT_BRACE, {}));
            break;
        case '}':
            scn.tokens.push_back(
              create_token(scn, token_type::RIGHT_BRACE, {}));
            break;
        case ',':
            scn.tokens.push_back(create_token(scn, token_type::COMMA, {}));
            break;
        case '.':
            scn.tokens.push_back(create_token(scn, token_type::DOT, {}));
            break;
        case '-':
            scn.tokens.push_back(create_token(scn, token_type::MINUS, {}));
            break;
        case '+':
            scn.tokens.push_back(create_token(scn, token_type::PLUS, {}));
            break;
        case ';':
            scn.tokens.push_back(create_token(scn, token_type::SEMICOLON, {}));
            break;
        case '*':
            scn.tokens.push_back(create_token(scn, token_type::STAR, {}));
            break;
        case '?':
            scn.tokens.push_back(
              create_token(scn, token_type::QUESTION_MARK, {}));
            break;
        case ':':
            scn.tokens.push_back(create_token(scn, token_type::COLON, {}));
            break;
        case '!':
            scn.tokens.push_back(
              match(scn, '=') ? create_token(scn, token_type::BANG_EQUAL, {})
                              : create_token(scn, token_type::BANG, {}));
            break;
        case '=':
            scn.tokens.push_back(
              match(scn, '=') ? create_token(scn, token_type::EQUAL_EQUAL, {})
                              : create_token(scn, token_type::EQUAL, {}));
            break;
        case '<':
            scn.tokens.push_back(
              match(scn, '=') ? create_token(scn, token_type::LESS_EQUAL, {})
                              : create_token(scn, token_type::LESS, {}));
            break;
        case '>':
            scn.tokens.push_back(
              match(scn, '=') ? create_token(scn, token_type::GREATER_EQUAL, {})
                              : create_token(scn, token_type::GREATER, {}));
            break;
        case '/':
            if (match(scn, '/') || peek(scn) == '*') {
                scan_comment(scn);
            }
            else {
                scn.tokens.push_back(create_token(scn, token_type::SLASH, {}));
            }
            break;
        case ' ':
            [[fallthrough]];
        case '\r':
            [[fallthrough]];
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            scn.line++;
            break;
        case '"':
            scan_string(scn);
            break;
        default:
            if (std::isdigit(ch)) {
                scan_number(scn);
            }
            else if (lox::isalpha(ch)) {
                scan_identifier(scn);
            }
            else {
                scn.has_error = true;
                log_error(scn, "Unknown token.");
            }
    }
}
}

std::vector<lox::token> lox::scan_tokens(std::string_view source) LOX_NOEXCEPT
{
    scan_data scn{ source };
    while (!is_at_end(scn)) {
        scn.start = scn.current;
        scan_tokens_impl(scn);
        if (scn.has_error) {
            std::cerr << "Stopped because of parsing errors.\n";
            break;
        }
    }

    if (scn.has_error) {
        return {};
    }
    else if (!scn.tokens.empty()) {
        scn.tokens.push_back(lox::token{ token_type::END_OF_FILE,
          "\0",
          {},
          scn.line,
          scn.current,
          scn.current,
          get_current_line_str(scn) });
    }

    return scn.tokens;
}
