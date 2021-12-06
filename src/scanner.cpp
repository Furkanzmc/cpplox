#include "scanner.h"

namespace {
bool is_at_end(const scanner& scn)
{
    return scn.current >= scn.source.size();
}

char advance(scanner& scn)
{
    return scn.source.at(scn.current++);
}

scanner scan_token(scanner scn)
{
    char ch = advance(scn);
    switch (ch) {
        case '(':
            scn.tokens.emplace_back(token::token_type::LEFT_PAREN,
                                    scn.source.substr(scn.start, scn.current),
                                    nullptr,
                                    scn.line);
            break;
    }

    return scn;
}
}

scanner scan_tokens(scanner scn)
{
    while (!is_at_end(scn)) {
        scn.start = scn.current;
        scn = scan_token(scn);
    }
    // scn.tokens.emplace_back(
    //   token::token_type::LEFT_PAREN, "", nullptr, scn.current);
    return scn;
}
