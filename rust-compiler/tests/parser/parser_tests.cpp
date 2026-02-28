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

TEST(ParserStatements, LetAndIfElse) {
  const char* input =
      "fn main(x: i32) {\n"
      "  let mut x = 1;\n"
      "  let y: i32 = x + 1;\n"
      "  if x > 0 { return; } else if x < 0 { return x; } else { x = x - 1; };\n"
      "}\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserStatements, BlockWithoutSemicolon) {
  const char* input =
      "fn main() {\n"
      "  if true { return; }\n"
      "  while true { return; }\n"
      "  { return; }\n"
      "  return;\n"
      "}\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserExpressions, Assignment) {
  const char* input = "fn main(x: i32) { x = x - 1; }";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserFunctions, EmptyBodyAndMultipleStatements) {
  const char* input =
      "fn empty() {}\n"
      "fn multi() { let x = 1; return x; }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserStatements, NestedCompoundStatements) {
  const char* input = "fn nested() { { { return; } } }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserExpressions, PrecedenceAndParentheses) {
  const char* input =
      "fn prec(a: i32, b: i32, c: i32) { return a * b + c; }\n"
      "fn prec2(a: i32, b: i32, c: i32) { return a * (b + c); }\n"
      "fn prec3(x: i32, y: i32, z: i32) { return x + y * z; }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserExpressions, Indexing) {
  const char* input =
      "fn index(a: i32, b: i32) { let x = arr[a]; let y = m[a][b]; return y; }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserStatements, ForAndWhileLoops) {
  const char* input =
      "fn loop_for() { for i in arr { return; } return; }\n"
      "fn loop_while(x: i32) { while x < 3 { return x; } }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_FALSE(had_error);
}

TEST(ParserErrors, ForMissingIn) {
  const char* input = "fn bad() { for i arr { return; } }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_TRUE(had_error);
}

TEST(ParserErrors, IndexMissingBracket) {
  const char* input = "fn bad() { let x = arr[1; }\n";
  bool had_error = false;
  std::unique_ptr<Program> program = ParseProgram(input, &had_error);
  ASSERT_TRUE(program);
  EXPECT_TRUE(had_error);
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
