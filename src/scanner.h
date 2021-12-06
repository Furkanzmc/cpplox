#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

#include <vector>
#include <string_view>

struct scanner {
    std::string_view source;
    std::vector<token> tokens;
    std::size_t start;
    std::size_t line;
    std::size_t current;
};

scanner scan_tokens(scanner scn);

#endif
