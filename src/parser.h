#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "token.h"
#include "expr.h"
#include "defs.h"

#include <vector>

namespace lox {
[[nodiscard]] std::vector<lox::stmt> parse(
  const std::vector<lox::token>& tokens) LOX_NOEXCEPT;
}

#endif
