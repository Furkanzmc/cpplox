#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "expr.h"

namespace lox {
object interpret(const expr& expression) LOX_NOEXCEPT;
};

#endif
