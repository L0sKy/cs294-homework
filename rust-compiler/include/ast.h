#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

struct DumpPrefix {
  std::string indent;
  bool is_last = true;
  bool is_root = false;

  static DumpPrefix Root() { return DumpPrefix{"", true, true}; }
};

class ASTNode {
 public:
  virtual ~ASTNode() = default;
  virtual void dump(std::ostream& out, const DumpPrefix& prefix) const = 0;
};

struct Decl : ASTNode {};
struct Stmt : ASTNode {};
struct Expr : ASTNode {};

struct Program : ASTNode {
  std::vector<std::unique_ptr<Decl>> decls;

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct Argument : ASTNode {
  std::string name;
  std::string type_name;
  size_t line = 0;
  size_t col = 0;

  Argument(std::string name, std::string type_name, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct FuncDecl : Decl {
  std::string name;
  std::vector<std::unique_ptr<Argument>> args;
  std::unique_ptr<struct CompoundStmt> body;
  size_t line = 0;
  size_t col = 0;

  FuncDecl(std::string name,
           std::vector<std::unique_ptr<Argument>> args,
           std::unique_ptr<CompoundStmt> body,
           size_t line,
           size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct CompoundStmt : Stmt {
  std::vector<std::unique_ptr<Stmt>> statements;
  size_t line = 0;
  size_t col = 0;

  CompoundStmt(std::vector<std::unique_ptr<Stmt>> statements, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct LetStmt : Stmt {
  std::string name;
  bool is_mutable = false;
  std::string type_name;
  std::unique_ptr<Expr> initializer;
  size_t line = 0;
  size_t col = 0;

  LetStmt(std::string name,
          bool is_mutable,
          std::string type_name,
          std::unique_ptr<Expr> initializer,
          size_t line,
          size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct IfStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<CompoundStmt> then_branch;
  std::unique_ptr<Stmt> else_branch;
  size_t line = 0;
  size_t col = 0;

  IfStmt(std::unique_ptr<Expr> condition,
         std::unique_ptr<CompoundStmt> then_branch,
         std::unique_ptr<Stmt> else_branch,
         size_t line,
         size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct ReturnStmt : Stmt {
  std::unique_ptr<Expr> value;
  size_t line = 0;
  size_t col = 0;

  ReturnStmt(std::unique_ptr<Expr> value, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct WhileStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<CompoundStmt> body;
  size_t line = 0;
  size_t col = 0;

  WhileStmt(std::unique_ptr<Expr> condition,
            std::unique_ptr<CompoundStmt> body,
            size_t line,
            size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct ExprStmt : Stmt {
  std::unique_ptr<Expr> expr;
  size_t line = 0;
  size_t col = 0;

  ExprStmt(std::unique_ptr<Expr> expr, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct BinaryExpr : Expr {
  std::string op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  size_t line = 0;
  size_t col = 0;

  BinaryExpr(std::string op,
             std::unique_ptr<Expr> lhs,
             std::unique_ptr<Expr> rhs,
             size_t line,
             size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct UnaryExpr : Expr {
  std::string op;
  std::unique_ptr<Expr> operand;
  size_t line = 0;
  size_t col = 0;

  UnaryExpr(std::string op, std::unique_ptr<Expr> operand, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct LiteralExpr : Expr {
  std::string value;
  size_t line = 0;
  size_t col = 0;

  LiteralExpr(std::string value, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct IdentExpr : Expr {
  std::string name;
  size_t line = 0;
  size_t col = 0;

  IdentExpr(std::string name, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct ParenExpr : Expr {
  std::unique_ptr<Expr> expr;
  size_t line = 0;
  size_t col = 0;

  ParenExpr(std::unique_ptr<Expr> expr, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};

struct AssignExpr : Expr {
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  size_t line = 0;
  size_t col = 0;

  AssignExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, size_t line, size_t col);

  void dump(std::ostream& out, const DumpPrefix& prefix) const override;
};
