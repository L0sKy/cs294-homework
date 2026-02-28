#include "lexer.h"

#include <cctype>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <unordered_set>

namespace {

const std::unordered_set<std::string> kKeywords = {
    "fn", "let", "mut", "if", "else", "while", "for", "in", "return",
};

const std::unordered_set<std::string> kBooleans = {"true", "false"};

class LexerImpl {
 public:
  explicit LexerImpl(std::string input) : input_(std::move(input)) {}

  std::vector<Token> Tokenize() {
    std::vector<Token> tokens;
    while (true) {
      if (auto error_token = SkipWhitespaceAndComments()) {
        tokens.push_back(*error_token);
        break;
      }
      if (IsAtEnd()) {
        break;
      }
      tokens.push_back(NextToken());
    }
    tokens.emplace_back(TokenType::Eof, "", line_, col_);
    return tokens;
  }

  bool had_error() const { return had_error_; }

 private:
  bool IsAtEnd() const { return index_ >= input_.size(); }

  char Peek() const { return IsAtEnd() ? '\0' : input_[index_]; }

  char PeekNext() const {
    if (index_ + 1 >= input_.size()) {
      return '\0';
    }
    return input_[index_ + 1];
  }

  char Advance() {
    if (IsAtEnd()) {
      return '\0';
    }
    char ch = input_[index_++];
    if (ch == '\n') {
      ++line_;
      col_ = 1;
    } else {
      ++col_;
    }
    return ch;
  }

  static bool IsIdentifierStart(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
  }

  static bool IsIdentifierPart(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
  }

  void ReportError(const std::string& message, size_t line, size_t col) {
    had_error_ = true;
    std::cerr << "Lexer error at " << line << ":" << col << ": " << message << '\n';
  }

  std::optional<Token> SkipWhitespaceAndComments() {
    while (!IsAtEnd()) {
      char ch = Peek();
      if (std::isspace(static_cast<unsigned char>(ch))) {
        Advance();
        continue;
      }
      if (ch == '/') {
        char next = PeekNext();
        if (next == '/') {
          SkipLineComment();
          continue;
        }
        if (next == '*') {
          auto error = SkipBlockComment();
          if (error) {
            return error;
          }
          continue;
        }
      }
      break;
    }
    return std::nullopt;
  }

  void SkipLineComment() {
    Advance();
    Advance();
    while (!IsAtEnd() && Peek() != '\n') {
      Advance();
    }
  }

  std::optional<Token> SkipBlockComment() {
    size_t start_line = line_;
    size_t start_col = col_;
    std::string value;
    value.push_back(Advance());
    value.push_back(Advance());

    int depth = 1;
    while (!IsAtEnd()) {
      char ch = Peek();
      if (ch == '/' && PeekNext() == '*') {
        value.push_back(Advance());
        value.push_back(Advance());
        ++depth;
        continue;
      }
      if (ch == '*' && PeekNext() == '/') {
        value.push_back(Advance());
        value.push_back(Advance());
        --depth;
        if (depth == 0) {
          return std::nullopt;
        }
        continue;
      }
      value.push_back(Advance());
    }

    ReportError("unterminated block comment", start_line, start_col);
    return Token(TokenType::Unknown, value, start_line, start_col);
  }

  Token NextToken() {
    size_t start_line = line_;
    size_t start_col = col_;
    char ch = Peek();

    if (IsIdentifierStart(ch)) {
      return LexIdentifier(start_line, start_col);
    }
    if (std::isdigit(static_cast<unsigned char>(ch))) {
      return LexNumber(start_line, start_col);
    }
    if (ch == '"') {
      return LexString(start_line, start_col);
    }
    if (ch == '\'') {
      return LexChar(start_line, start_col);
    }

    if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
      return LexMaybeTwoCharOperator(start_line, start_col);
    }

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      Advance();
      return Token(TokenType::Operator, std::string(1, ch), start_line, start_col);
    }

    if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ';' ||
        ch == ':' || ch == ',') {
      Advance();
      return Token(TokenType::Punctuation, std::string(1, ch), start_line, start_col);
    }

    Advance();
    ReportError("unknown token", start_line, start_col);
    return Token(TokenType::Unknown, std::string(1, ch), start_line, start_col);
  }

  Token LexIdentifier(size_t start_line, size_t start_col) {
    std::string value;
    while (!IsAtEnd() && IsIdentifierPart(Peek())) {
      value.push_back(Advance());
    }
    if (kBooleans.count(value) != 0u) {
      return Token(TokenType::BooleanLiteral, value, start_line, start_col);
    }
    if (kKeywords.count(value) != 0u) {
      return Token(TokenType::Keyword, value, start_line, start_col);
    }
    return Token(TokenType::Identifier, value, start_line, start_col);
  }

  Token LexNumber(size_t start_line, size_t start_col) {
    std::string value;
    while (!IsAtEnd() && std::isdigit(static_cast<unsigned char>(Peek()))) {
      value.push_back(Advance());
    }
    return Token(TokenType::NumberLiteral, value, start_line, start_col);
  }

  Token LexString(size_t start_line, size_t start_col) {
    std::string value;
    value.push_back(Advance());
    while (!IsAtEnd()) {
      char ch = Advance();
      value.push_back(ch);
      if (ch == '"') {
        return Token(TokenType::StringLiteral, value, start_line, start_col);
      }
      if (ch == '\n') {
        ReportError("unterminated string literal", start_line, start_col);
        return Token(TokenType::Unknown, value, start_line, start_col);
      }
    }
    ReportError("unterminated string literal", start_line, start_col);
    return Token(TokenType::Unknown, value, start_line, start_col);
  }

  Token LexChar(size_t start_line, size_t start_col) {
    std::string value;
    value.push_back(Advance());
    if (IsAtEnd()) {
      ReportError("unterminated char literal", start_line, start_col);
      return Token(TokenType::Unknown, value, start_line, start_col);
    }

    char ch = Advance();
    value.push_back(ch);
    if (ch == '\n' || IsAtEnd()) {
      ReportError("unterminated char literal", start_line, start_col);
      return Token(TokenType::Unknown, value, start_line, start_col);
    }

    if (Peek() == '\'') {
      value.push_back(Advance());
      return Token(TokenType::CharLiteral, value, start_line, start_col);
    }

    ReportError("unterminated char literal", start_line, start_col);
    return Token(TokenType::Unknown, value, start_line, start_col);
  }

  Token LexMaybeTwoCharOperator(size_t start_line, size_t start_col) {
    char first = Advance();
    if (Peek() == '=') {
      char second = Advance();
      std::string value;
      value.push_back(first);
      value.push_back(second);
      return Token(TokenType::Operator, value, start_line, start_col);
    }
    if (first == '!') {
      ReportError("unknown token", start_line, start_col);
      return Token(TokenType::Unknown, std::string(1, first), start_line, start_col);
    }
    return Token(TokenType::Operator, std::string(1, first), start_line, start_col);
  }

  std::string input_;
  size_t index_ = 0;
  size_t line_ = 1;
  size_t col_ = 1;
  bool had_error_ = false;
};

}  // namespace

Token::Token(TokenType type, std::string value, size_t line, size_t col)
    : type_(type), value_(std::move(value)), line_(line), col_(col) {}

TokenType Token::type() const { return type_; }
const std::string& Token::value() const { return value_; }
size_t Token::line() const { return line_; }
size_t Token::col() const { return col_; }

Lexer::Lexer(std::istream& input) : input_(input) {}

std::vector<Token> Lexer::tokenize() {
  had_error_ = false;
  std::string buffer;
  input_.seekg(0, std::ios::end);
  std::streamoff size = input_.tellg();
  if (size > 0) {
    buffer.resize(static_cast<size_t>(size));
    input_.seekg(0, std::ios::beg);
    input_.read(&buffer[0], size);
  } else {
    input_.clear();
    input_.seekg(0, std::ios::beg);
    buffer.assign(std::istreambuf_iterator<char>(input_), std::istreambuf_iterator<char>());
  }

  LexerImpl impl(std::move(buffer));
  std::vector<Token> tokens = impl.Tokenize();
  had_error_ = impl.had_error();
  return tokens;
}

bool Lexer::had_error() const { return had_error_; }
