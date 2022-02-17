#ifndef LOX_AST_PRINTER_H
#define LOX_AST_PRINTER_H

#include "expr.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const lox::expr& expr) LOX_NOEXCEPT;
std::ostream& operator<<(std::ostream& os,
  const lox::stmt& statement) LOX_NOEXCEPT;
std::ostream& operator<<(std::ostream& os,
  const lox::object& object) LOX_NOEXCEPT;

#endif
