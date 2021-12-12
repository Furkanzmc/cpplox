#ifndef LOG_H
#define LOG_H

#include <string_view>

struct scanner;

void log_error(const scanner& scn, std::string_view message);

#endif
