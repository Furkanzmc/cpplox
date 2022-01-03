#ifndef LOX_UTILS_H
#define LOX_UTILS_H

#include "defs.h"

#include <cctype>
#include <iostream>
#include <iomanip>

namespace lox {

inline bool isalnum(char ch) LOX_NOEXCEPT
{
    return std::isalnum(ch) || ch == '_';
}

inline bool isalpha(char ch) LOX_NOEXCEPT
{
    return std::isalpha(ch) || ch == '_';
}

inline void log_error(std::string_view line_str,
  std::size_t linenr,
  std::size_t column_start,
  std::string_view message) LOX_NOEXCEPT
{
    std::cerr << "Error: " << message << '\n'
              << std::setfill(' ') << std::setw(4) << linenr << "| " << line_str
              << '\n'
              << std::setfill(' ') << std::setw(column_start + 4 + 1 + 1 + 1)
              << "^"
              << "-- Here" << '\n';
}

}

#endif
