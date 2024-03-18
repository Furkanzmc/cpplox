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
#include <cassert>

namespace {
constexpr std::string_view s_version{ "v0.0.0.1" };

struct arguments {
    bool verbose{ false };
    std::string_view file_path{};
};

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

arguments parse_args(int argc, char** argv) noexcept
{
    arguments args{};
    for (int i = 1; i < argc; ++i) {
        if (std::string_view{ "--verbose" } == argv[i]) {
            args.verbose = true;
        }
        else {
            args.file_path = argv[i];
        }
    }

    return args;
}

void interpret(const std::vector<lox::stmt>& statements,
  lox::environment& env,
  bool exit_on_error,
  bool is_verbose) LOX_NOEXCEPT
{
    assert(!statements.empty());
    for (const auto& stmt : statements) {
        if (is_verbose) {
            std::clog << stmt << '\n';
        }

        try {
            const lox::object result = lox::interpret(stmt, env);

            if (is_verbose) {
                std::clog << result << "\n";
            }
        }
        catch (lox::runtime_error& ex) {
            std::clog << "Runtime error: " << ex.what() << "\n";
            if (exit_on_error) {
                exit(EX_SOFTWARE);
            }
        }
    }
}

void run_file(std::string_view file_path, bool is_verbose) LOX_NOEXCEPT
{
    const std::ifstream reader{ file_path, std::ifstream::in };
    if (!reader.is_open()) {
        std::cerr << "Cannot open '" << file_path << "' for readin!\n";
        exit(EX_NOINPUT);
    }

    std::stringstream stream;
    stream << reader.rdbuf();
    const std::string content{ stream.str() };
    const auto result = lox::scan_tokens(content);
    if (!result.tokens.empty()) {
        const auto statements = lox::parse(result.tokens);
        if (!statements.empty()) {
            lox::environment env{};
            interpret(statements, env, true, is_verbose);
        }
    }
}

void run_prompt(bool is_verbose) LOX_NOEXCEPT
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
                interpret(statements, env, false, is_verbose);
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

    const auto args = parse_args(argc, argv);
    if (args.file_path.empty()) {
        run_prompt(args.verbose);
    }
    else {
        run_file(args.file_path, args.verbose);
    }

    return EX_OK;
}
