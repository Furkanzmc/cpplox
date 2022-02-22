#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "defs.h"

#include <unordered_map>
#include <string>

namespace lox {

struct token;

struct environment {
    std::unordered_map<std::string, lox::object> values;
};

namespace env {
void define(environment& env, std::string name, lox::object value) LOX_NOEXCEPT;

// Raises lox::runtime_error
lox::object get(const environment& env, const lox::token& name);
}

}

#endif
