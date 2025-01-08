#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "expr.h"
#include "exceptions.h"

#include <zmcx/expected.h>

namespace lox {
struct environment;

/*!
 * @throws lox::runtime_error
 */
zx::expected<object, lox::runtime_error> interpret(const stmt& statement,
  environment& env);
};

#endif
