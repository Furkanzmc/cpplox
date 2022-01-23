#include "scanner.h"

#include "parser.h"
#include "defs.h"
#include "ast_printer.h"

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
      { ".version", [](std::string_view /* command */) {
           std::clog << s_version << '\n';
       } } };

void run_file(std::string_view file_path) LOX_NOEXCEPT
{
    std::ifstream reader{ file_path, std::ifstream::in };
    if (!reader.is_open()) {
        std::cerr << "Cannot open '" << file_path << "' for readin!\n";
        exit(EX_NOINPUT);
    }

    std::stringstream content;
    content << reader.rdbuf();
    const auto tokens = lox::scan_tokens(content.str());
    if (!tokens.empty()) {
        const auto expr = lox::parse(tokens);
        if (!std::holds_alternative<std::monostate>(expr)) {
            std::clog << expr << '\n';
        }
    }
}

void run_prompt() LOX_NOEXCEPT
{
    std::string input{};
    std::cout << "Welcome to Lox++ " << s_version << '.' << '\n' << "> ";
    while (std::getline(std::cin, input)) {
        const auto foundReplIt{ s_repl_commands.find(input.c_str()) };
        if (foundReplIt != s_repl_commands.cend()) {
            assert(foundReplIt->second);
            foundReplIt->second(input.c_str());
        }
        else {
            const auto tokens = lox::scan_tokens(input.c_str());
            if (!tokens.empty()) {
                const auto expr = lox::parse(tokens);
                if (!std::holds_alternative<std::monostate>(expr)) {
                    std::clog << expr << '\n';
                }
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
