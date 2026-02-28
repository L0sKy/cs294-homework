#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "lexer.h"

class Parser {
 public:
  explicit Parser(std::vector<Token> tokens);

  std::unique_ptr<Program> parse_program();
  bool had_error() const;

 private:
  const Token& peek(int k = 0) const;
  const Token& advance();
  bool is_at_end() const;

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
  std::unique_ptr<Stmt> parse_for_stmt();
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
  std::unique_ptr<Expr> parse_postfix();
  std::unique_ptr<Expr> parse_primary();

  void synchronize();
  void report_error(const Token& token, const std::string& message);

  std::vector<Token> tokens_;
  size_t index_ = 0;
  bool had_error_ = false;
};
