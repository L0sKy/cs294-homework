#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "ast.h"
#include "lexer.h"
#include "parser.h"

namespace {

std::unique_ptr<Program> ParseProgram(const std::string& input, bool* had_error) {
  std::istringstream stream(input);
  Lexer lexer(stream);
  std::vector<Token> tokens = lexer.tokenize();
  Parser parser(std::move(tokens));
  std::unique_ptr<Program> program = parser.parse_program();
  if (had_error) {
    *had_error = parser.had_error() || lexer.had_error();
  }
  return program;
}

std::string DumpProgram(const Program& program) {
  std::ostringstream out;
  program.dump(out, DumpPrefix::Root());
  return out.str();
}

}  // namespace

TEST(ParserDump, FunctionWithReturn) {
  const char* input = "fn add(x: i32, y: i32) { return x + y; }";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);

  const char* expected =
      "TranslationUnitDecl\n"
      "`-FunctionDecl <1:1> add\n"
      "  |-ParmVarDecl <1:8> x : i32\n"
      "  |-ParmVarDecl <1:16> y : i32\n"
      "  `-CompoundStmt <1:24>\n"
      "    `-ReturnStmt <1:26>\n"
      "      `-BinaryOperator '+' <1:35>\n"
      "        |-DeclRefExpr <1:33> x\n"
      "        `-DeclRefExpr <1:37> y\n";

  EXPECT_EQ(DumpProgram(*program), expected);
}

TEST(ParserStatements, WhileAndExpr) {
  const char* input =
      "fn main(x: i32) {\n"
      "  while x < 3 { return x; };\n"
      "  x + 1;\n"
      "}\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserErrors, MissingBrace) {
  const char* input = "fn main(x: i32) { return x;";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_TRUE(had_error);
}

TEST(ParserErrors, MissingParen) {
  const char* input = "fn main(x: i32 { return; }";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_TRUE(had_error);
}
