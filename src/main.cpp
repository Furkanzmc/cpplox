#include "scanner.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <map>
#include <sysexits.h>

namespace {
const std::map<std::string_view,
  std::function<void(std::string_view /*command*/)>>
  s_repl_commands{ { ".exit",
                     [](std::string_view /* command */) {
                         exit(EX_OK);
                     } },
      { ".version", [](std::string_view /* command */) {
           std::clog << "v0.0.0.1\n";
       } } };

void run_file(std::string_view file_path)
{
    std::ifstream reader{ file_path, std::ifstream::in };
    if (!reader.is_open()) {
        std::cerr << "Cannot open '" << file_path << "' for readin!\n";
        exit(EX_NOINPUT);
    }

    std::stringstream content;
    content << reader.rdbuf();
    scanner scn = scan_tokens(content.str());
    for (const auto& token : scn.tokens) {
        std::clog << token << '\n';
    }
}

void run_prompt()
{
    std::string input{};
    std::cout << "> ";
    while (std::getline(std::cin, input)) {
        const auto foundReplIt{ s_repl_commands.find(input.c_str()) };
        if (foundReplIt != s_repl_commands.cend()) {
            assert(foundReplIt->second);
            foundReplIt->second(input.c_str());
        }
        else {
            scanner scn = scan_tokens(input.c_str());
            if (!scn.has_error) {
                for (const auto& token : scn.tokens) {
                    std::clog << token << '\n';
                }
            }
        }

        input.clear();
        std::cout << "> ";
    }
}
}

int main(int argc, char** argv)
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