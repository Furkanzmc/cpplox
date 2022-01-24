# !/usr/bin/python3
from typing import List
from os import system

EXPRESSIONS = {
    "binary": [
        "expr left",
        "token oprtor",
        "expr right",
    ],
    "ternary": [
        "expr first",
        "expr second",
        "expr third",
    ],
    "grouping": ["expr expression"],
    "literal": ["object value"],
    "unary": ["token oprtor", "expr right"],
}


def generate() -> List[str]:
    outputs: List[str] = [
        """
/*
 _____ _     _        __ _ _        _                   _
|_   _| |__ (_)___   / _(_) | ___  (_)___    __ _ _   _| |_ ___
  | | | '_ \\| / __| | |_| | |/ _ \\ | / __|  / _` | | | | __/ _ \\
  | | | | | | \\__ \\ |  _| | |  __/ | \\__ \\ | (_| | |_| | || (_) |
  |_| |_| |_|_|___/ |_| |_|_|\\___| |_|___/  \\__,_|\\__,_|\\__\\___/

                                 _           _
  __ _  ___ _ __   ___ _ __ __ _| |_ ___  __| |
 / _` |/ _ \\ '_ \\ / _ \\ '__/ _` | __/ _ \\/ _` |
| (_| |  __/ | | |  __/ | | (_| | ||  __/ (_| |_
 \\__, |\\___|_| |_|\\___|_|  \\__,_|\\__\\___|\\__,_(_)
 |___/
 ____                      _              _ _ _   _
|  _ \\  ___    _ __   ___ | |_    ___  __| (_) |_| |
| | | |/ _ \\  | '_ \\ / _ \\| __|  / _ \\/ _` | | __| |
| |_| | (_) | | | | | (_) | |_  |  __/ (_| | | |_|_|
|____/ \\___/  |_| |_|\\___/ \\__|  \\___|\\__,_|_|\\__(_)

*/
""",
        "#ifndef LOX_EXPR_H",
        "#define LOX_EXPR_H",
        "",
        '#include "token.h"',
        "",
        "#include <memory>",
        "",
        "namespace lox {",
    ]

    for key in EXPRESSIONS:
        outputs.append(f"struct {key};")

    outputs.append("")
    types: List[str] = ["std::unique_ptr<" + key + ">" for key in EXPRESSIONS]
    outputs.append(f"using expr = std::variant<std::monostate, {','.join(types)}>;")
    outputs.append("")

    for key in EXPRESSIONS:
        content = [f"struct {key}" "{"]
        for type in EXPRESSIONS[key]:
            cmps: List[str] = type.split(" ")
            content.append(f"{cmps[0]} {cmps[1]};")

        content.extend(["};", ""])
        outputs.extend(content)

    outputs.append("}")
    outputs.append("")
    outputs.append("#endif")
    return outputs


def main():
    content: str = "\n".join(generate())
    with open("src/expr.h", "w") as file_handle:
        file_handle.write(content)

    system("clang-format -i src/expr.h")


if __name__ == "__main__":
    main()
