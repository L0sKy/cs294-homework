#include "parser.h"

#include <iostream>

namespace {

bool IsKeyword(const Token& token, const std::string& value) {
  return token.type() == TokenType::Keyword && token.value() == value;
}

bool IsPunct(const Token& token, const std::string& value) {
  return token.type() == TokenType::Punctuation && token.value() == value;
}

bool IsOperatorToken(const Token& token, const std::vector<std::string>& ops) {
  if (token.type() != TokenType::Operator) {
    return false;
  }
  for (const auto& op : ops) {
    if (token.value() == op) {
      return true;
    }
  }
  return false;
}

}  // namespace

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::unique_ptr<Program> Parser::parse_program() {
  auto program = std::make_unique<Program>();
  while (!is_at_end()) {
    std::unique_ptr<Decl> decl = parse_decl();
    if (decl) {
      program->decls.push_back(std::move(decl));
      continue;
    }
    synchronize();
  }
  return program;
}

bool Parser::had_error() const { return had_error_; }

const Token& Parser::peek(int k) const {
  size_t idx = index_ + static_cast<size_t>(k);
  if (idx >= tokens_.size()) {
    return tokens_.back();
  }
  return tokens_[idx];
}

const Token& Parser::advance() {
  if (!is_at_end()) {
    ++index_;
  }
  return tokens_[index_ - 1];
}

bool Parser::is_at_end() const { return peek().type() == TokenType::Eof; }

bool Parser::match(TokenType type, const std::string& value) {
  const Token& token = peek();
  if (token.type() != type) {
    return false;
  }
  if (!value.empty() && token.value() != value) {
    return false;
  }
  advance();
  return true;
}

const Token& Parser::expect(TokenType type, const std::string& message,
                            const std::string& value) {
  const Token& token = peek();
  if (token.type() == type && (value.empty() || token.value() == value)) {
    return advance();
  }
  report_error(token, message);
  return advance();
}

std::unique_ptr<Decl> Parser::parse_decl() {
  if (IsKeyword(peek(), "fn")) {
    return parse_func_decl();
  }
  report_error(peek(), "expected declaration");
  return nullptr;
}

std::unique_ptr<FuncDecl> Parser::parse_func_decl() {
  const Token& fn_token = expect(TokenType::Keyword, "expected 'fn'", "fn");
  const Token& name_token = expect(TokenType::Identifier, "expected function name");
  expect(TokenType::Punctuation, "expected '('", "(");
  std::vector<std::unique_ptr<Argument>> args = parse_argument_list();
  expect(TokenType::Punctuation, "expected ')'", ")");
  std::unique_ptr<CompoundStmt> body = parse_compound_statement();

  return std::make_unique<FuncDecl>(name_token.value(), std::move(args), std::move(body),
                                    fn_token.line(), fn_token.col());
}

std::vector<std::unique_ptr<Argument>> Parser::parse_argument_list() {
  std::vector<std::unique_ptr<Argument>> args;
  if (IsPunct(peek(), ")")) {
    return args;
  }
  args.push_back(parse_argument());
  while (match(TokenType::Punctuation, ",")) {
    args.push_back(parse_argument());
  }
  return args;
}

std::unique_ptr<Argument> Parser::parse_argument() {
  const Token& name_token = expect(TokenType::Identifier, "expected argument name");
  expect(TokenType::Punctuation, "expected ':'", ":");
  const Token& type_token = expect(TokenType::Identifier, "expected type name");
  return std::make_unique<Argument>(name_token.value(), type_token.value(), name_token.line(),
                                    name_token.col());
}

std::unique_ptr<CompoundStmt> Parser::parse_compound_statement() {
  const Token& lbrace = expect(TokenType::Punctuation, "expected '{'", "{");
  std::vector<std::unique_ptr<Stmt>> statements;
  if (!IsPunct(peek(), "}")) {
    statements = parse_statement_list();
  }
  expect(TokenType::Punctuation, "expected '}'", "}");
  return std::make_unique<CompoundStmt>(std::move(statements), lbrace.line(), lbrace.col());
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_statement_list() {
  std::vector<std::unique_ptr<Stmt>> statements;
  bool needs_semicolon = false;
  while (!IsPunct(peek(), "}") && !is_at_end()) {
    const Token& start = peek();
    std::unique_ptr<Stmt> stmt = parse_statement();
    bool is_block = false;
    if (IsKeyword(start, "if") || IsKeyword(start, "while") || IsKeyword(start, "for") ||
        IsPunct(start, "{")) {
      is_block = true;
    }
    statements.push_back(std::move(stmt));

    if (IsPunct(peek(), "}")) {
      break;
    }
    if (is_block) {
      match(TokenType::Punctuation, ";");
      continue;
    }
    if (!match(TokenType::Punctuation, ";")) {
      report_error(peek(), "expected ';'");
      synchronize();
      if (IsPunct(peek(), "}")) {
        break;
      }
    }
  }
  return statements;
}

std::unique_ptr<Stmt> Parser::parse_statement() {
  if (IsKeyword(peek(), "let")) {
    return parse_let_stmt();
  }
  if (IsKeyword(peek(), "for")) {
    return parse_for_stmt();
  }
  if (IsKeyword(peek(), "if")) {
    return parse_if_stmt();
  }
  if (IsKeyword(peek(), "return")) {
    return parse_return_stmt();
  }
  if (IsKeyword(peek(), "while")) {
    return parse_while_stmt();
  }
  if (IsPunct(peek(), "{")) {
    return parse_compound_statement();
  }
  return parse_expr_stmt();
}

std::unique_ptr<Stmt> Parser::parse_let_stmt() {
  const Token& kw = expect(TokenType::Keyword, "expected 'let'", "let");
  bool is_mutable = false;
  if (IsKeyword(peek(), "mut")) {
    advance();
    is_mutable = true;
  }
  const Token& name = expect(TokenType::Identifier, "expected identifier");
  std::string type_name;
  if (match(TokenType::Punctuation, ":")) {
    const Token& type_token = expect(TokenType::Identifier, "expected type name");
    type_name = type_token.value();
  }
  expect(TokenType::Operator, "expected '='", "=");
  std::unique_ptr<Expr> init = parse_expression();
  return std::make_unique<LetStmt>(name.value(), is_mutable, type_name, std::move(init),
                                   kw.line(), kw.col());
}

std::unique_ptr<Stmt> Parser::parse_for_stmt() {
  const Token& kw = expect(TokenType::Keyword, "expected 'for'", "for");
  const Token& name = expect(TokenType::Identifier, "expected loop variable");
  if (IsKeyword(peek(), "in")) {
    advance();
  } else {
    report_error(peek(), "expected 'in'");
  }
  std::unique_ptr<Expr> iterable = parse_expression();
  std::unique_ptr<CompoundStmt> body = parse_compound_statement();
  return std::make_unique<ForStmt>(name.value(), std::move(iterable), std::move(body), kw.line(),
                                   kw.col());
}

std::unique_ptr<Stmt> Parser::parse_if_stmt() {
  const Token& kw = expect(TokenType::Keyword, "expected 'if'", "if");
  std::unique_ptr<Expr> condition = parse_expression();
  std::unique_ptr<CompoundStmt> then_branch = parse_compound_statement();
  std::unique_ptr<Stmt> else_branch;
  if (IsKeyword(peek(), "else")) {
    advance();
    if (IsKeyword(peek(), "if")) {
      else_branch = parse_if_stmt();
    } else {
      else_branch = parse_compound_statement();
    }
  }
  return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),
                                  std::move(else_branch), kw.line(), kw.col());
}

std::unique_ptr<Stmt> Parser::parse_return_stmt() {
  const Token& kw = expect(TokenType::Keyword, "expected 'return'", "return");
  if (IsPunct(peek(), ";") || IsPunct(peek(), "}")) {
    return std::make_unique<ReturnStmt>(nullptr, kw.line(), kw.col());
  }
  std::unique_ptr<Expr> value = parse_expression();
  return std::make_unique<ReturnStmt>(std::move(value), kw.line(), kw.col());
}

std::unique_ptr<Stmt> Parser::parse_while_stmt() {
  const Token& kw = expect(TokenType::Keyword, "expected 'while'", "while");
  std::unique_ptr<Expr> condition = parse_expression();
  std::unique_ptr<CompoundStmt> body = parse_compound_statement();
  return std::make_unique<WhileStmt>(std::move(condition), std::move(body), kw.line(), kw.col());
}

std::unique_ptr<Stmt> Parser::parse_expr_stmt() {
  const Token& token = peek();
  std::unique_ptr<Expr> expr = parse_expression();
  return std::make_unique<ExprStmt>(std::move(expr), token.line(), token.col());
}

std::unique_ptr<Expr> Parser::parse_expression() { return parse_assignment(); }

std::unique_ptr<Expr> Parser::parse_assignment() {
  std::unique_ptr<Expr> expr = parse_equality();
  if (IsOperatorToken(peek(), {"="})) {
    const Token& op = advance();
    std::unique_ptr<Expr> rhs = parse_assignment();
    return std::make_unique<AssignExpr>(std::move(expr), std::move(rhs), op.line(), op.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_equality() {
  std::unique_ptr<Expr> expr = parse_comparison();
  while (IsOperatorToken(peek(), {"==", "!="})) {
    const Token& op = advance();
    std::unique_ptr<Expr> rhs = parse_comparison();
    expr = std::make_unique<BinaryExpr>(op.value(), std::move(expr), std::move(rhs), op.line(),
                                        op.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_comparison() {
  std::unique_ptr<Expr> expr = parse_term();
  while (IsOperatorToken(peek(), {"<", ">", "<=", ">="})) {
    const Token& op = advance();
    std::unique_ptr<Expr> rhs = parse_term();
    expr = std::make_unique<BinaryExpr>(op.value(), std::move(expr), std::move(rhs), op.line(),
                                        op.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_term() {
  std::unique_ptr<Expr> expr = parse_factor();
  while (IsOperatorToken(peek(), {"+", "-"})) {
    const Token& op = advance();
    std::unique_ptr<Expr> rhs = parse_factor();
    expr = std::make_unique<BinaryExpr>(op.value(), std::move(expr), std::move(rhs), op.line(),
                                        op.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_factor() {
  std::unique_ptr<Expr> expr = parse_unary();
  while (IsOperatorToken(peek(), {"*", "/"})) {
    const Token& op = advance();
    std::unique_ptr<Expr> rhs = parse_unary();
    expr = std::make_unique<BinaryExpr>(op.value(), std::move(expr), std::move(rhs), op.line(),
                                        op.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_unary() {
  if (IsOperatorToken(peek(), {"+", "-"})) {
    const Token& op = advance();
    std::unique_ptr<Expr> operand = parse_unary();
    return std::make_unique<UnaryExpr>(op.value(), std::move(operand), op.line(), op.col());
  }
  return parse_postfix();
}

std::unique_ptr<Expr> Parser::parse_postfix() {
  std::unique_ptr<Expr> expr = parse_primary();
  while (match(TokenType::Punctuation, "[")) {
    const Token& lbracket = tokens_[index_ - 1];
    std::unique_ptr<Expr> index_expr = parse_expression();
    expect(TokenType::Punctuation, "expected ']'", "]");
    expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index_expr), lbracket.line(),
                                       lbracket.col());
  }
  return expr;
}

std::unique_ptr<Expr> Parser::parse_primary() {
  const Token& token = peek();
  if (token.type() == TokenType::Identifier) {
    advance();
    return std::make_unique<IdentExpr>(token.value(), token.line(), token.col());
  }
  if (token.type() == TokenType::NumberLiteral || token.type() == TokenType::StringLiteral ||
      token.type() == TokenType::CharLiteral || token.type() == TokenType::BooleanLiteral) {
    advance();
    return std::make_unique<LiteralExpr>(token.value(), token.line(), token.col());
  }
  if (match(TokenType::Punctuation, "(")) {
    const Token& lparen = tokens_[index_ - 1];
    std::unique_ptr<Expr> expr = parse_expression();
    expect(TokenType::Punctuation, "expected ')'", ")");
    return std::make_unique<ParenExpr>(std::move(expr), lparen.line(), lparen.col());
  }
  report_error(token, "expected expression");
  advance();
  return std::make_unique<LiteralExpr>("0", token.line(), token.col());
}

void Parser::synchronize() {
  bool advanced = false;
  while (!is_at_end()) {
    if (match(TokenType::Punctuation, ";")) {
      return;
    }
    if (IsPunct(peek(), "}")) {
      if (!advanced) {
        advance();
      }
      return;
    }
    if (IsKeyword(peek(), "fn")) {
      if (!advanced) {
        advance();
      }
      return;
    }
    advance();
    advanced = true;
  }
}

void Parser::report_error(const Token& token, const std::string& message) {
  had_error_ = true;
  std::cerr << "Parser error at " << token.line() << ":" << token.col() << ": " << message
            << '\n';
}
