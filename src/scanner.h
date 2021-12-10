#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "defs.h"

#include <vector>
#include <string_view>

struct scanner {
    std::string_view source;
    std::vector<token> tokens{};
    std::size_t start{ 0 };
    std::size_t line{ 0 };
    std::size_t current{ 0 };
    bool has_error{ false };
};

scanner scan_tokens(std::string_view source) LOX_NOEXCEPT;

#endif
