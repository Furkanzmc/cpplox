#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"
#include "defs.h"

#include <vector>
#include <string_view>

namespace lox {
[[nodiscard]] std::vector<token> scan_tokens(
  std::string_view source) LOX_NOEXCEPT;
}

#endif
