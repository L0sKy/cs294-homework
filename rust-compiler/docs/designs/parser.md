# Parser

This document describes the parser module of rust-compiler.
The parser consumes lexer tokens and constructs an AST (Abstract Syntax Tree).

## Introduction

- Input: token sequence from `Lexer`.
- Output: AST for declarations and statements.
- Method: hand-written recursive descent with one-token lookahead.

## Grammar Subset

This phase does not include `for` loops or array indexing.

```ebnf
program             := decl* EOF

decl                := func_decl

func_decl            := "fn" identifier "(" argument_list? ")" compound_statement
argument_list        := argument ("," argument)*
argument             := identifier ":" identifier

compound_statement   := "{" statement_list? "}"
statement_list       := statement (statement_sep statement)*
statement_sep        := ";"
                     | <implicit>  // allowed after block statements

statement            := let_stmt
                      | if_stmt
                      | return_stmt
                      | while_stmt
                      | compound_statement
                      | expr_stmt

let_stmt             := "let" "mut"? identifier (":" identifier)? "=" expression
if_stmt              := "if" expression compound_statement ("else" (if_stmt | compound_statement))?

return_stmt          := "return" expression?
while_stmt           := "while" expression compound_statement
expr_stmt            := expression

expression           := assignment

assignment           := equality ( "=" assignment )?

equality             := comparison ( ("==" | "!=") comparison )*
comparison           := term ( ("<" | ">" | "<=" | ">=") term )*
term                 := factor ( ("+" | "-") factor )*
factor               := unary ( ("*" | "/") unary )*
unary                := ("+" | "-") unary | primary
primary              := identifier
                      | literal
                      | "(" expression ")"

literal              := number | string | char | boolean
```

## AST Data Structures

The AST is built during parsing.

```cpp
struct ASTNode {
  virtual ~ASTNode() = default;
  virtual void dump(std::ostream& out, const DumpPrefix& prefix) const = 0;
};

struct Program : ASTNode {
  std::vector<std::unique_ptr<Decl>> decls;
};

struct Decl : ASTNode {};

struct FuncDecl : Decl {
  std::string name;
  std::vector<std::unique_ptr<Argument>> args;
  std::unique_ptr<CompoundStmt> body;
  size_t line;
  size_t col;
};

struct Argument : ASTNode {
  std::string name;
  std::string type_name;
  size_t line;
  size_t col;
};

struct Stmt : ASTNode {};

struct CompoundStmt : Stmt {
  std::vector<std::unique_ptr<Stmt>> statements;
  size_t line;
  size_t col;
};

struct LetStmt : Stmt {
  std::string name;
  bool is_mutable;
  std::string type_name;  // empty if not specified
  std::unique_ptr<Expr> initializer;
  size_t line;
  size_t col;
};

struct IfStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<CompoundStmt> then_branch;
  std::unique_ptr<Stmt> else_branch;  // may be nullptr
  size_t line;
  size_t col;
};

struct ReturnStmt : Stmt {
  std::unique_ptr<Expr> value;  // null for "return;"
  size_t line;
  size_t col;
};

struct WhileStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<CompoundStmt> body;
  size_t line;
  size_t col;
};

struct ExprStmt : Stmt {
  std::unique_ptr<Expr> expr;
  size_t line;
  size_t col;
};

struct Expr : ASTNode {};

struct BinaryExpr : Expr {
  std::string op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  size_t line;
  size_t col;
};

struct UnaryExpr : Expr {
  std::string op;
  std::unique_ptr<Expr> operand;
  size_t line;
  size_t col;
};

struct LiteralExpr : Expr {
  std::string value;
  size_t line;
  size_t col;
};

struct IdentExpr : Expr {
  std::string name;
  size_t line;
  size_t col;
};

struct ParenExpr : Expr {
  std::unique_ptr<Expr> expr;
  size_t line;
  size_t col;
};

struct AssignExpr : Expr {
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  size_t line;
  size_t col;
};
```

## Parser API

```cpp
class Parser {
 public:
  explicit Parser(std::vector<Token> tokens);

  std::unique_ptr<Program> parse_program();
  bool had_error() const;

 private:
  const Token& peek(int k = 0) const;
  bool match(TokenType type, const std::string& value = "");
  const Token& expect(TokenType type, const std::string& message,
                      const std::string& value = "");

  std::unique_ptr<Decl> parse_decl();
  std::unique_ptr<FuncDecl> parse_func_decl();
  std::vector<std::unique_ptr<Argument>> parse_argument_list();
  std::unique_ptr<Argument> parse_argument();

  std::unique_ptr<CompoundStmt> parse_compound_statement();
  std::vector<std::unique_ptr<Stmt>> parse_statement_list();
  std::unique_ptr<Stmt> parse_statement();
  std::unique_ptr<Stmt> parse_let_stmt();
  std::unique_ptr<Stmt> parse_if_stmt();
  std::unique_ptr<Stmt> parse_return_stmt();
  std::unique_ptr<Stmt> parse_while_stmt();
  std::unique_ptr<Stmt> parse_expr_stmt();

  std::unique_ptr<Expr> parse_expression();
  std::unique_ptr<Expr> parse_assignment();
  std::unique_ptr<Expr> parse_equality();
  std::unique_ptr<Expr> parse_comparison();
  std::unique_ptr<Expr> parse_term();
  std::unique_ptr<Expr> parse_factor();
  std::unique_ptr<Expr> parse_unary();
  std::unique_ptr<Expr> parse_primary();

  void synchronize();
};
```

- `peek`: lookahead without consuming.
- `match`: consume when current token matches type (and optional value).
- `expect`: enforce syntax rule or record error.

Keywords are still tokens of `TokenType::Keyword`; the parser checks `Token::value()`
for `fn`, `return`, `while`.

## Error Handling and Recovery

- Syntax errors include line and column from the current token.
- `had_error()` returns true if any parse error occurred.
- Recovery: when a statement-level error occurs, advance until `;`, `}`, or `fn` to
  resume parsing.

## AST Dump Format

Each AST subclass implements `dump()` and prints only itself, then recursively
calls `dump()` on children.

- Root prints `TranslationUnitDecl` with no leading connector.
- Child nodes print with clang-style prefixes:
  - `|-` for non-last child
  - ``-` for last child
- Each level adds indentation; children are always more indented than their parent.

Example:

```text
TranslationUnitDecl
`-FunctionDecl <1:1> add
  |-ParmVarDecl <1:9> x : i32
  |-ParmVarDecl <1:16> y : i32
  `-CompoundStmt <1:22>
    `-ReturnStmt <1:24>
      `-BinaryOperator '+' <1:31>
        |-DeclRefExpr <1:31> x
        `-DeclRefExpr <1:35> y
```
