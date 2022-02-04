# !/usr/bin/python3
from typing import List
from os import system

EXPRESSIONS = {
    "binary": [
        "copyable<expr*> left",
        "token oprtor",
        "copyable<expr*> right",
    ],
    "ternary": [
        "copyable<expr*> first",
        "copyable<expr*> second",
        "copyable<expr*> third",
    ],
    "grouping": ["copyable<expr*> expression"],
    "literal": ["object value"],
    "unary": ["token oprtor", "copyable<expr*> right"],
}


def generate() -> List[str]:
    outputs: List[str] = [
        "// Auto generated. DO NOT EDIT!",
        "#ifndef LOX_EXPR_H",
        "#define LOX_EXPR_H",
        "",
        '#include "defs.h"',
        '#include "token.h"',
        "",
        "#include <memory>",
        "",
        "namespace lox {",
        "struct expr;",
    ]

    outputs.append("")

    for key in EXPRESSIONS:
        content = [f"struct {key}" "{"]
        for type in EXPRESSIONS[key]:
            cmps: List[str] = type.split(" ")
            content.append(f"{cmps[0]} {cmps[1]};")

        content.extend(["};", ""])
        outputs.extend(content)

    types: List[str] = [key for key in EXPRESSIONS]
    outputs.append(
        "struct expr : public std::variant<std::monostate," + ",".join(types) + "> {"
    )
    outputs.append("using variant::variant;};")
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
