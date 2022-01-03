#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "token.h"
#include "expr.h"
#include "defs.h"

#include <vector>

namespace lox {
[[nodiscard]] lox::expr parse(
  const std::vector<lox::token>& tokens) LOX_NOEXCEPT;
}

#endif
