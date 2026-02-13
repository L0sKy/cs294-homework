#pragma once

#include <cstddef>
#include <istream>
#include <string>
#include <vector>

enum class TokenType {
  Keyword,
  Identifier,
  NumberLiteral,
  StringLiteral,
  CharLiteral,
  BooleanLiteral,
  Operator,
  Punctuation,
  Eof,
  Unknown,
};

class Token {
 public:
  Token(TokenType type, std::string value, size_t line, size_t col);

  TokenType type() const;
  const std::string& value() const;
  size_t line() const;
  size_t col() const;

 private:
  TokenType type_;
  std::string value_;
  size_t line_;
  size_t col_;
};

class Lexer {
 public:
  explicit Lexer(std::istream& input);

  std::vector<Token> tokenize();

 private:
  std::istream& input_;
};
