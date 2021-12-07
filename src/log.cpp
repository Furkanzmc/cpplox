#include "log.h"

#include <iostream>

void log_error(int line, std::string_view message)
{
    std::cerr << "Error at line " << line << ": " << message << '\n';
}
