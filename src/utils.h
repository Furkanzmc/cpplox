#ifndef LOX_UTILS_H
#define LOX_UTILS_H

#include "defs.h"

#include <cctype>

namespace lox {

inline bool isalnum(char ch) LOX_NOEXCEPT
{
    return std::isalnum(ch) || ch == '_';
}

inline bool isalpha(char ch) LOX_NOEXCEPT
{
    return std::isalpha(ch) || ch == '_';
}

}

#endif
