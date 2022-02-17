# Lox++

C++ implementation of Lox language from [craftinginterpreters](https://craftinginterpreters.com).

## Grammar

```
program        → statement* EOF ;
statement      → expressionStmt | printStmt ;
expressionStmt → ternary ";";
printStmt      → "print" ternary ";";
ternary        → expression | (ternary)* "?" expression | (ternary)* ":" expression | (ternary)* | primary
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
```
