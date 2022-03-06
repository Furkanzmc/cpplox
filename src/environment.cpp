#include "environment.h"

#include "token.h"
#include "exceptions.h"

namespace lox {
namespace env {
void define(lox::environment& env,
  std::string name,
  lox::object value) LOX_NOEXCEPT
{
    assert(env.values.find(name) == env.values.end());

    env.values.emplace(std::move(name), std::move(value));
}

lox::object get(const environment& env, const lox::token& name)
{
    const auto foundIt = std::find_if(env.values.cbegin(),
      env.values.cend(),
      [&name](const auto& pair) { return pair.first == name.lexeme; });

    if (foundIt == env.values.cend()) {
        throw lox::runtime_error{ name,
            "Undefined variable '" + std::string{ name.lexeme } + "'." };
    }

    return foundIt->second;
}

void assign(environment& env, const lox::token& name, lox::object value)
{
    const auto foundIt = std::find_if(env.values.cbegin(),
      env.values.cend(),
      [&name](const auto& pair) { return pair.first == name.lexeme; });

    if (foundIt == env.values.cend()) {
        throw lox::runtime_error{ name,
            "Undefined variable '" + std::string{ name.lexeme } + "'." };
    }

    std::string var_name{ name.lexeme.data(), name.lexeme.size() };
    env.values[var_name] = std::move(value);
}
}
}
