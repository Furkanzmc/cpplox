#include "log.h"
#include "scanner.h"
#include "defs.h"

#include <iostream>
#include <iomanip>

namespace {
std::string_view get_error_line(const scanner& scn)
{
    assert(scn.has_error);
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
}

void log_error(const scanner& scn, std::string_view message)
{
    const std::string linenr{ std::to_string(scn.line + 1) };
    const std::string_view line_str{ get_error_line(scn) };

    std::cerr << "Error: " << message << '\n'
              << std::setfill(' ') << std::setw(4) << linenr << "| " << line_str
              << '\n'
              << std::setfill(' ') << std::setw(scn.start + 4 + 1 + 1 + 1)
              << "^"
              << "-- Here" << '\n';
}
