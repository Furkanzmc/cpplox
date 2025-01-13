#ifndef LOX_EXCEPTIONS_H
#define LOX_EXCEPTIONS_H

#include "expr.h"

#include <string>

namespace lox {
class runtime_error {
public:
    lox::token token;
    std::string message;

public:
    runtime_error() = default;

    runtime_error(lox::token _token, std::string msg)
      : token{ std::move(_token) }
      , message{ std::move(msg) }
    {
    }

    const char* what() const noexcept
    {
        return message.c_str();
    }
};
}

#endif
