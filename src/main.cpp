#include "scanner.h"

#include "parser.h"
#include "ast_printer.h"
#include "interpreter.h"
#include "environment.h"
#include "exceptions.h"
#include "defs.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <map>
#include <sysexits.h>

namespace {
constexpr std::string_view s_version{ "v0.0.0.1" };

const std::map<std::string_view,
  std::function<void(std::string_view /*command*/)>>
  s_repl_commands{ { ".exit",
                     [](std::string_view /* command */) {
                         exit(EX_OK);
                     } },
      { ".help",
        [](std::string_view /* command */) {
            for (const auto& it : s_repl_commands) {
                if (it.first != ".help") {
                    std::clog << it.first << '\n';
                }
            }
        } },
      { ".version", [](std::string_view /* command */) {
           std::clog << s_version << '\n';
       } } };

void interpret(const std::vector<lox::stmt>& statements,
  lox::environment& env,
  bool exit_on_error) LOX_NOEXCEPT
{
    assert(!statements.empty());
    for (const auto& stmt : statements) {
        std::clog << stmt << '\n';
        try {
            const lox::object result = lox::interpret(stmt, env);
            std::clog << result << "\n";
        }
        catch (lox::runtime_error&) {
            if (exit_on_error) {
                exit(EX_SOFTWARE);
            }
        }
    }
}

void run_file(std::string_view file_path) LOX_NOEXCEPT
{
    const std::ifstream reader{ file_path, std::ifstream::in };
    if (!reader.is_open()) {
        std::cerr << "Cannot open '" << file_path << "' for readin!\n";
        exit(EX_NOINPUT);
    }

    std::stringstream content;
    content << reader.rdbuf();
    const auto result = lox::scan_tokens(content.str());
    if (!result.tokens.empty()) {
        const auto statements = lox::parse(result.tokens);
        if (!statements.empty()) {
            lox::environment env{};
            interpret(statements, env, true);
        }
    }
}

void run_prompt() LOX_NOEXCEPT
{
    std::string input{};
    std::cout << "Welcome to Lox++ " << s_version << '.' << '\n' << "> ";
    lox::environment env{};
    while (std::getline(std::cin, input)) {
        const auto foundReplIt{ s_repl_commands.find(input.c_str()) };
        if (foundReplIt != s_repl_commands.cend()) {
            assert(foundReplIt->second);
            foundReplIt->second(input.c_str());
        }
        else {
            const auto result = lox::scan_tokens(input.c_str());
            if (!result.tokens.empty()) {
                const auto statements = lox::parse(result.tokens);
                interpret(statements, env, false);
            }
        }

        input.clear();
        std::cout << "> ";
    }
}
}

int main(int argc, char** argv) LOX_NOEXCEPT
{
    if (argc > 2) {
        std::cerr << "Usage: lox++ [script]\n";
        return EX_USAGE;
    }
    else if (argc == 2) {
        run_file(argv[1]);
    }
    else {
        run_prompt();
    }

    return EX_OK;
}
