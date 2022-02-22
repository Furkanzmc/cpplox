#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "expr.h"

namespace lox {
struct environment;

// Raises lox::runtime_error.
object interpret(const stmt& statement, environment& env);
};

#endif
