#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "lexer.h"

namespace {

std::vector<Token> Tokenize(const std::string& input) {
  std::istringstream stream(input);
  Lexer lexer(stream);
  return lexer.tokenize();
}

void ExpectToken(const Token& token, TokenType type, const std::string& value, size_t line,
                 size_t col) {
  EXPECT_EQ(token.type(), type);
  EXPECT_EQ(token.value(), value);
  EXPECT_EQ(token.line(), line);
  EXPECT_EQ(token.col(), col);
}

}  // namespace

TEST(LexerKeywords, RecognizesKeywords) {
  auto tokens = Tokenize("fn let mut if else while return");
  ASSERT_GE(tokens.size(), 8u);

  ExpectToken(tokens[0], TokenType::Keyword, "fn", 1, 1);
  ExpectToken(tokens[1], TokenType::Keyword, "let", 1, 4);
  ExpectToken(tokens[2], TokenType::Keyword, "mut", 1, 8);
  ExpectToken(tokens[3], TokenType::Keyword, "if", 1, 12);
  ExpectToken(tokens[4], TokenType::Keyword, "else", 1, 15);
  ExpectToken(tokens[5], TokenType::Keyword, "while", 1, 20);
  ExpectToken(tokens[6], TokenType::Keyword, "return", 1, 26);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerIdentifiers, RecognizesIdentifiers) {
  auto tokens = Tokenize("_x foo Bar_2 a1");
  ASSERT_GE(tokens.size(), 5u);

  ExpectToken(tokens[0], TokenType::Identifier, "_x", 1, 1);
  ExpectToken(tokens[1], TokenType::Identifier, "foo", 1, 4);
  ExpectToken(tokens[2], TokenType::Identifier, "Bar_2", 1, 8);
  ExpectToken(tokens[3], TokenType::Identifier, "a1", 1, 14);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerLiterals, RecognizesLiterals) {
  auto tokens = Tokenize("0 42 7 \"hello\" \"\" \"hello world\" 'a' 'Z' true false");
  ASSERT_GE(tokens.size(), 11u);

  ExpectToken(tokens[0], TokenType::NumberLiteral, "0", 1, 1);
  ExpectToken(tokens[1], TokenType::NumberLiteral, "42", 1, 3);
  ExpectToken(tokens[2], TokenType::NumberLiteral, "7", 1, 6);
  ExpectToken(tokens[3], TokenType::StringLiteral, "\"hello\"", 1, 8);
  ExpectToken(tokens[4], TokenType::StringLiteral, "\"\"", 1, 16);
  ExpectToken(tokens[5], TokenType::StringLiteral, "\"hello world\"", 1, 19);
  ExpectToken(tokens[6], TokenType::CharLiteral, "'a'", 1, 33);
  ExpectToken(tokens[7], TokenType::CharLiteral, "'Z'", 1, 37);
  ExpectToken(tokens[8], TokenType::BooleanLiteral, "true", 1, 41);
  ExpectToken(tokens[9], TokenType::BooleanLiteral, "false", 1, 46);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerOperators, RecognizesOperatorsGreedy) {
  auto tokens = Tokenize("+ - * / = == != < > <= >=");
  ASSERT_GE(tokens.size(), 12u);

  ExpectToken(tokens[0], TokenType::Operator, "+", 1, 1);
  ExpectToken(tokens[1], TokenType::Operator, "-", 1, 3);
  ExpectToken(tokens[2], TokenType::Operator, "*", 1, 5);
  ExpectToken(tokens[3], TokenType::Operator, "/", 1, 7);
  ExpectToken(tokens[4], TokenType::Operator, "=", 1, 9);
  ExpectToken(tokens[5], TokenType::Operator, "==", 1, 11);
  ExpectToken(tokens[6], TokenType::Operator, "!=", 1, 14);
  ExpectToken(tokens[7], TokenType::Operator, "<", 1, 17);
  ExpectToken(tokens[8], TokenType::Operator, ">", 1, 19);
  ExpectToken(tokens[9], TokenType::Operator, "<=", 1, 21);
  ExpectToken(tokens[10], TokenType::Operator, ">=", 1, 24);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerPunctuation, RecognizesPunctuation) {
  auto tokens = Tokenize("( ) { } ; : ,");
  ASSERT_GE(tokens.size(), 8u);

  ExpectToken(tokens[0], TokenType::Punctuation, "(", 1, 1);
  ExpectToken(tokens[1], TokenType::Punctuation, ")", 1, 3);
  ExpectToken(tokens[2], TokenType::Punctuation, "{", 1, 5);
  ExpectToken(tokens[3], TokenType::Punctuation, "}", 1, 7);
  ExpectToken(tokens[4], TokenType::Punctuation, ";", 1, 9);
  ExpectToken(tokens[5], TokenType::Punctuation, ":", 1, 11);
  ExpectToken(tokens[6], TokenType::Punctuation, ",", 1, 13);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerPositions, TracksLineAndColumn) {
  const char* input =
      "fn main() {\n"
      "  let x = 42;\n"
      "}\n";

  auto tokens = Tokenize(input);
  ASSERT_GE(tokens.size(), 10u);

  ExpectToken(tokens[0], TokenType::Keyword, "fn", 1, 1);
  ExpectToken(tokens[1], TokenType::Identifier, "main", 1, 4);
  ExpectToken(tokens[2], TokenType::Punctuation, "(", 1, 8);
  ExpectToken(tokens[3], TokenType::Punctuation, ")", 1, 9);
  ExpectToken(tokens[4], TokenType::Punctuation, "{", 1, 11);
  ExpectToken(tokens[5], TokenType::Keyword, "let", 2, 3);
  ExpectToken(tokens[6], TokenType::Identifier, "x", 2, 7);
  ExpectToken(tokens[7], TokenType::Operator, "=", 2, 9);
  ExpectToken(tokens[8], TokenType::NumberLiteral, "42", 2, 11);
  ExpectToken(tokens[9], TokenType::Punctuation, ";", 2, 13);

  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}

TEST(LexerUnknown, ReportsUnknownTokens) {
  auto tokens = Tokenize("@");
  ASSERT_GE(tokens.size(), 2u);

  ExpectToken(tokens[0], TokenType::Unknown, "@", 1, 1);
  EXPECT_EQ(tokens.back().type(), TokenType::Eof);
}
