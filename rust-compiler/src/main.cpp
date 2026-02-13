#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
  args.add_argument("-f", "--file")
      .help("Input Rust source file path.")
      .default_value(std::string(""));
  args.add_argument("--lexer")
      .help("Output lexer tokens in the format: TYPE value line col.")
      .default_value(false)
      .implicit_value(true);
  args.add_argument("-h", "--help")
      .help("Show this help message and exit.")
      .default_value(false)
      .implicit_value(true);
  args.add_argument("input").remaining();

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

  const std::vector<std::string> positional_inputs =
      args.present<std::vector<std::string>>("input").value_or(std::vector<std::string>{});
  if (positional_inputs.size() > 1) {
    std::cerr << "Only one positional input file is allowed.\n";
    std::cerr << args;
    return 1;
  }

  std::string input_file = args.get<std::string>("--file");
  if (!input_file.empty() && !positional_inputs.empty()) {
    std::cerr << "Do not provide both --file/-f and a positional input file.\n";
    std::cerr << args;
    return 1;
  }
  if (input_file.empty() && !positional_inputs.empty()) {
    input_file = positional_inputs[0];
  }
  if (input_file.empty()) {
    std::cerr << "--file/-f is required unless one positional input file is provided.\n";
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

  const bool show_lexer = args.get<bool>("--lexer");
  if (show_lexer) {
    for (const auto& token : tokens) {
      std::cout << TokenTypeToString(token.type()) << ' ' << token.value() << ' '
                << token.line() << ' ' << token.col() << '\n';
    }
  }

  if (lexer.had_error()) {
    return 1;
  }

  return 0;
}
