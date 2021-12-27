#ifndef LOX_AST_PRINTER_H
#define LOX_AST_PRINTER_H

#include "expr.h"
#include "defs.h"

[[nodiscard]] std::string print_ast(const expr& ex) LOX_NOEXCEPT;

#endif
