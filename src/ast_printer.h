#ifndef LOX_AST_PRINTER_H
#define LOX_AST_PRINTER_H

#include "expr.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const lox::expr& expr);
std::ostream& operator<<(std::ostream& os, const lox::object& object);

#endif
