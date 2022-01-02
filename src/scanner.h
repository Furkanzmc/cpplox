#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "defs.h"

#include <vector>
#include <string_view>

[[nodiscard]] std::vector<token> scan_tokens(
  std::string_view source) LOX_NOEXCEPT;

#endif
