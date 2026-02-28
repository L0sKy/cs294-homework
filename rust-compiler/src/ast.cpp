#include "ast.h"

#include <sstream>
#include <utility>

namespace {

std::string FormatLocation(size_t line, size_t col) {
  std::ostringstream out;
  out << '<' << line << ':' << col << '>';
  return out.str();
}

void DumpLine(std::ostream& out, const DumpPrefix& prefix, const std::string& label) {
  if (prefix.is_root) {
    out << label << '\n';
    return;
  }
  out << prefix.indent << (prefix.is_last ? "`-" : "|-") << label << '\n';
}

DumpPrefix ChildPrefix(const DumpPrefix& prefix, bool child_is_last) {
  DumpPrefix child;
  child.is_root = false;
  child.is_last = child_is_last;
  if (prefix.is_root) {
    child.indent = "";
    return child;
  }
  child.indent = prefix.indent + (prefix.is_last ? "  " : "| ");
  return child;
}

std::string LiteralKind(const std::string& value) {
  if (value == "true" || value == "false") {
    return "BooleanLiteral";
  }
  if (!value.empty() && value.front() == '"') {
    return "StringLiteral";
  }
  if (!value.empty() && value.front() == '\'') {
    return "CharLiteral";
  }
  return "IntegerLiteral";
}

}  // namespace

Argument::Argument(std::string name, std::string type_name, size_t line, size_t col)
    : name(std::move(name)), type_name(std::move(type_name)), line(line), col(col) {}

FuncDecl::FuncDecl(std::string name,
                   std::vector<std::unique_ptr<Argument>> args,
                   std::unique_ptr<CompoundStmt> body,
                   size_t line,
                   size_t col)
    : name(std::move(name)),
      args(std::move(args)),
      body(std::move(body)),
      line(line),
      col(col) {}

CompoundStmt::CompoundStmt(std::vector<std::unique_ptr<Stmt>> statements,
                           size_t line,
                           size_t col)
    : statements(std::move(statements)), line(line), col(col) {}

LetStmt::LetStmt(std::string name,
                 bool is_mutable,
                 std::string type_name,
                 std::unique_ptr<Expr> initializer,
                 size_t line,
                 size_t col)
    : name(std::move(name)),
      is_mutable(is_mutable),
      type_name(std::move(type_name)),
      initializer(std::move(initializer)),
      line(line),
      col(col) {}

IfStmt::IfStmt(std::unique_ptr<Expr> condition,
               std::unique_ptr<CompoundStmt> then_branch,
               std::unique_ptr<Stmt> else_branch,
               size_t line,
               size_t col)
    : condition(std::move(condition)),
      then_branch(std::move(then_branch)),
      else_branch(std::move(else_branch)),
      line(line),
      col(col) {}

ForStmt::ForStmt(std::string name,
                 std::unique_ptr<Expr> iterable,
                 std::unique_ptr<CompoundStmt> body,
                 size_t line,
                 size_t col)
    : name(std::move(name)),
      iterable(std::move(iterable)),
      body(std::move(body)),
      line(line),
      col(col) {}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> value, size_t line, size_t col)
    : value(std::move(value)), line(line), col(col) {}

WhileStmt::WhileStmt(std::unique_ptr<Expr> condition,
                     std::unique_ptr<CompoundStmt> body,
                     size_t line,
                     size_t col)
    : condition(std::move(condition)), body(std::move(body)), line(line), col(col) {}

ExprStmt::ExprStmt(std::unique_ptr<Expr> expr, size_t line, size_t col)
    : expr(std::move(expr)), line(line), col(col) {}

BinaryExpr::BinaryExpr(std::string op,
                       std::unique_ptr<Expr> lhs,
                       std::unique_ptr<Expr> rhs,
                       size_t line,
                       size_t col)
    : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)), line(line), col(col) {}

UnaryExpr::UnaryExpr(std::string op, std::unique_ptr<Expr> operand, size_t line, size_t col)
    : op(std::move(op)), operand(std::move(operand)), line(line), col(col) {}

LiteralExpr::LiteralExpr(std::string value, size_t line, size_t col)
    : value(std::move(value)), line(line), col(col) {}

IdentExpr::IdentExpr(std::string name, size_t line, size_t col)
    : name(std::move(name)), line(line), col(col) {}

ParenExpr::ParenExpr(std::unique_ptr<Expr> expr, size_t line, size_t col)
    : expr(std::move(expr)), line(line), col(col) {}

AssignExpr::AssignExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, size_t line, size_t col)
    : lhs(std::move(lhs)), rhs(std::move(rhs)), line(line), col(col) {}

IndexExpr::IndexExpr(std::unique_ptr<Expr> target,
                     std::unique_ptr<Expr> index,
                     size_t line,
                     size_t col)
    : target(std::move(target)), index(std::move(index)), line(line), col(col) {}

void Program::dump(std::ostream& out, const DumpPrefix& prefix) const {
  DumpLine(out, prefix, "TranslationUnitDecl");
  for (size_t i = 0; i < decls.size(); ++i) {
    const bool is_last = (i + 1 == decls.size());
    decls[i]->dump(out, ChildPrefix(prefix, is_last));
  }
}

void Argument::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label =
      "ParmVarDecl " + FormatLocation(line, col) + " " + name + " : " + type_name;
  DumpLine(out, prefix, label);
}

void FuncDecl::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "FunctionDecl " + FormatLocation(line, col) + " " + name;
  DumpLine(out, prefix, label);

  size_t total_children = args.size() + (body ? 1u : 0u);
  size_t index = 0;
  for (const auto& arg : args) {
    const bool is_last = (index + 1 == total_children);
    arg->dump(out, ChildPrefix(prefix, is_last));
    ++index;
  }
  if (body) {
    const bool is_last = (index + 1 == total_children);
    body->dump(out, ChildPrefix(prefix, is_last));
  }
}

void CompoundStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "CompoundStmt " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  for (size_t i = 0; i < statements.size(); ++i) {
    const bool is_last = (i + 1 == statements.size());
    statements[i]->dump(out, ChildPrefix(prefix, is_last));
  }
}

void LetStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  std::string label = "LetStmt " + FormatLocation(line, col) + " ";
  if (is_mutable) {
    label += "mut ";
  }
  label += name;
  if (!type_name.empty()) {
    label += " : " + type_name;
  }
  DumpLine(out, prefix, label);
  if (initializer) {
    initializer->dump(out, ChildPrefix(prefix, true));
  }
}

void IfStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "IfStmt " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  size_t total_children = 0;
  if (condition) {
    ++total_children;
  }
  if (then_branch) {
    ++total_children;
  }
  if (else_branch) {
    ++total_children;
  }

  size_t index = 0;
  if (condition) {
    const bool is_last = (index + 1 == total_children);
    condition->dump(out, ChildPrefix(prefix, is_last));
    ++index;
  }
  if (then_branch) {
    const bool is_last = (index + 1 == total_children);
    then_branch->dump(out, ChildPrefix(prefix, is_last));
    ++index;
  }
  if (else_branch) {
    const bool is_last = (index + 1 == total_children);
    else_branch->dump(out, ChildPrefix(prefix, is_last));
  }
}

void ForStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "ForStmt " + FormatLocation(line, col) + " " + name;
  DumpLine(out, prefix, label);

  size_t total_children = 0;
  if (iterable) {
    ++total_children;
  }
  if (body) {
    ++total_children;
  }

  size_t index = 0;
  if (iterable) {
    const bool is_last = (index + 1 == total_children);
    iterable->dump(out, ChildPrefix(prefix, is_last));
    ++index;
  }
  if (body) {
    const bool is_last = (index + 1 == total_children);
    body->dump(out, ChildPrefix(prefix, is_last));
  }
}

void ReturnStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "ReturnStmt " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (value) {
    value->dump(out, ChildPrefix(prefix, true));
  }
}

void WhileStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "WhileStmt " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (condition && body) {
    condition->dump(out, ChildPrefix(prefix, false));
    body->dump(out, ChildPrefix(prefix, true));
  } else if (condition) {
    condition->dump(out, ChildPrefix(prefix, true));
  } else if (body) {
    body->dump(out, ChildPrefix(prefix, true));
  }
}

void ExprStmt::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "ExprStmt " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (expr) {
    expr->dump(out, ChildPrefix(prefix, true));
  }
}

void BinaryExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "BinaryOperator '" + op + "' " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (lhs && rhs) {
    lhs->dump(out, ChildPrefix(prefix, false));
    rhs->dump(out, ChildPrefix(prefix, true));
  } else if (lhs) {
    lhs->dump(out, ChildPrefix(prefix, true));
  } else if (rhs) {
    rhs->dump(out, ChildPrefix(prefix, true));
  }
}

void UnaryExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "UnaryOperator '" + op + "' " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (operand) {
    operand->dump(out, ChildPrefix(prefix, true));
  }
}

void LiteralExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label =
      LiteralKind(value) + " " + FormatLocation(line, col) + " " + value;
  DumpLine(out, prefix, label);
}

void IdentExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "DeclRefExpr " + FormatLocation(line, col) + " " + name;
  DumpLine(out, prefix, label);
}

void ParenExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "ParenExpr " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (expr) {
    expr->dump(out, ChildPrefix(prefix, true));
  }
}

void AssignExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "AssignExpr '=' " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (lhs && rhs) {
    lhs->dump(out, ChildPrefix(prefix, false));
    rhs->dump(out, ChildPrefix(prefix, true));
  } else if (lhs) {
    lhs->dump(out, ChildPrefix(prefix, true));
  } else if (rhs) {
    rhs->dump(out, ChildPrefix(prefix, true));
  }
}

void IndexExpr::dump(std::ostream& out, const DumpPrefix& prefix) const {
  const std::string label = "IndexExpr " + FormatLocation(line, col);
  DumpLine(out, prefix, label);
  if (target && index) {
    target->dump(out, ChildPrefix(prefix, false));
    index->dump(out, ChildPrefix(prefix, true));
  } else if (target) {
    target->dump(out, ChildPrefix(prefix, true));
  } else if (index) {
    index->dump(out, ChildPrefix(prefix, true));
  }
}
