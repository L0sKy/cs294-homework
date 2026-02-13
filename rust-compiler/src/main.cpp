#include <fstream>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

#include "lexer.h"

namespace {

std::string TokenTypeToString(TokenType type) {
  switch (type) {
    case TokenType::Keyword:
      return "Keyword";
    case TokenType::Identifier:
      return "Identifier";
    case TokenType::NumberLiteral:
      return "NumberLiteral";
    case TokenType::StringLiteral:
      return "StringLiteral";
    case TokenType::CharLiteral:
      return "CharLiteral";
    case TokenType::BooleanLiteral:
      return "BooleanLiteral";
    case TokenType::Operator:
      return "Operator";
    case TokenType::Punctuation:
      return "Punctuation";
    case TokenType::Eof:
      return "Eof";
    case TokenType::Unknown:
      return "Unknown";
  }
  return "Unknown";
}

}  // namespace

int main(int argc, char** argv) {
  argparse::ArgumentParser args("rustcompiler");
  args.add_argument("--file").default_value(std::string(""));
  args.add_argument("--lexer").default_value(false).implicit_value(true);
  args.add_argument("--help").default_value(false).implicit_value(true);

  try {
    args.parse_args(argc, argv);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::cerr << args;
    return 1;
  }

  if (args.get<bool>("--help")) {
    std::cout << args;
    return 0;
  }

  std::string input_file = args.get<std::string>("--file");
  if (input_file.empty()) {
    std::cerr << "--file is required\n";
    std::cerr << args;
    return 1;
  }

  std::ifstream input(input_file);
  if (!input.is_open()) {
    std::cerr << "Failed to open file: " << input_file << '\n';
    return 1;
  }

  Lexer lexer(input);
  std::vector<Token> tokens = lexer.tokenize();

  if (args.get<bool>("--lexer")) {
    for (const auto& token : tokens) {
      std::cout << TokenTypeToString(token.type()) << ' ' << token.value() << ' '
                << token.line() << ' ' << token.col() << '\n';
    }
  }

  return 0;
}
