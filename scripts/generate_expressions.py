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
    "variable": ["token name"],
    "assignment": ["token name", "copyable<expr*> value"],
}


STATEMENTS = {
    "expr_stmt": ["copyable<expr*> expression"],
    "print_stmt": ["copyable<expr*> expression"],
    "var_stmt": ["token name", "copyable<expr*> expression"],
}


def _create_structs(container: list):
    for key in container:
        content = [f"struct {key}" "{"]
        for typ in container[key]:
            cmps: List[str] = typ.split(" ")
            content.append(f"{cmps[0]} {cmps[1]};")

        content.extend(["};", ""])

        yield content


def generate() -> List[str]:
    outputs: List[str] = [
        "// Auto generated. DO NOT EDIT!",
        "#ifndef LOX_EXPR_H",
        "#define LOX_EXPR_H",
        "",
        '#include "defs.h"',
        '#include "token.h"',
        "",
        "namespace lox {",
        "struct expr;",
    ]

    outputs.append("")

    for struct in _create_structs(EXPRESSIONS):
        outputs.extend(struct)

    for struct in _create_structs(STATEMENTS):
        outputs.extend(struct)

    # Expressions
    types: List[str] = list(EXPRESSIONS)
    outputs.append(
        "struct expr : public std::variant<std::monostate," + ",".join(types) + "> {"
    )
    outputs.append("using variant::variant;};")
    outputs.append("")

    # Statements
    types: List[str] = list(STATEMENTS)
    outputs.append(
        "struct stmt : public std::variant<std::monostate," + ",".join(types) + "> {"
    )
    outputs.append("using variant::variant;};")

    outputs.append("}")
    outputs.append("")
    outputs.append("#endif")
    return outputs


def main():
    content: str = "\n".join(generate())
    with open("src/expr.h", "w", encoding="utf-8") as file_handle:
        file_handle.write(content)

    system("clang-format -i src/expr.h")


if __name__ == "__main__":
    main()
