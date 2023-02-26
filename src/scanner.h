#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"
#include "defs.h"

#include <vector>
#include <string_view>

namespace lox {
struct scan_result {
    struct error {
        enum class error_type {
            UNTERMINATED_STRING,
            UNKNOWN_TOKEN,
        };

        std::string message;
        std::size_t line;
        error_type type;

        error(error_type type, std::size_t ln);
    };

    static_assert(std::is_copy_assignable_v<error>, "Requirement error.");
    static_assert(std::is_copy_constructible_v<error>, "Requirement error.");
    static_assert(std::is_move_assignable_v<error>, "Requirement error.");
    static_assert(std::is_move_constructible_v<error>, "Requirement error.");

    std::vector<lox::token> tokens;
    std::vector<error> errors;
};

[[nodiscard]] scan_result scan_tokens(std::string_view source) LOX_NOEXCEPT;

}

#endif
