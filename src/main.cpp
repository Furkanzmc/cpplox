#include <iostream>
#include <fstream>
#include <sstream>
#include <string_view>
#include <sysexits.h>

namespace {
void run_file(std::string_view /* file_path */)
{
}

void run_prompt()
{
    std::string input{};
    std::cout << "> ";
    while (std::getline(std::cin, input)) {
        std::cout << input << '\n';
        std::cout << "> ";
        input.clear();
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
