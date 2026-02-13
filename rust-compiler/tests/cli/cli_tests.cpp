#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/wait.h>
#endif

#ifndef RUSTCC_BIN
#error "RUSTCC_BIN must be defined to the rustcc executable path."
#endif

namespace {

struct CommandResult {
  int exit_code = -1;
  std::string stdout_text;
  std::string stderr_text;
};

int DecodeExitCode(int raw_status) {
#ifdef _WIN32
  return raw_status;
#else
  if (raw_status == -1) {
    return -1;
  }
  return WEXITSTATUS(raw_status);
#endif
}

std::string ReadFile(const std::filesystem::path& path) {
  std::ifstream input(path);
  std::ostringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

std::filesystem::path MakeTempPath(const std::string& name_prefix, const std::string& suffix) {
  const auto now =
      std::chrono::steady_clock::now().time_since_epoch().count();
  return std::filesystem::temp_directory_path() /
         (name_prefix + "-" + std::to_string(now) + suffix);
}

void WriteTextFile(const std::filesystem::path& path, const std::string& content) {
  std::ofstream output(path);
  output << content;
}

CommandResult RunRustcc(const std::vector<std::string>& args) {
  const std::filesystem::path stdout_path = MakeTempPath("rustcc-stdout", ".txt");
  const std::filesystem::path stderr_path = MakeTempPath("rustcc-stderr", ".txt");

  std::ostringstream command;
  command << '"' << RUSTCC_BIN << '"';
  for (const auto& arg : args) {
    command << " \"" << arg << '"';
  }
  command << " > \"" << stdout_path.string() << "\" 2> \"" << stderr_path.string() << '"';

  const int raw_status = std::system(command.str().c_str());

  CommandResult result;
  result.exit_code = DecodeExitCode(raw_status);
  result.stdout_text = ReadFile(stdout_path);
  result.stderr_text = ReadFile(stderr_path);

  std::error_code ec;
  std::filesystem::remove(stdout_path, ec);
  std::filesystem::remove(stderr_path, ec);
  return result;
}

std::filesystem::path WriteRustFile(const std::string& content) {
  const std::filesystem::path file_path = MakeTempPath("rustcc-input", ".rs");
  WriteTextFile(file_path, content);
  return file_path;
}

}  // namespace

TEST(MainCliHelp, PrintsHelpAndExitsZero) {
  const CommandResult result = RunRustcc({"-h"});
  EXPECT_EQ(result.exit_code, 0);
  EXPECT_NE(result.stdout_text.find("--file"), std::string::npos);
}

TEST(MainCliHelp, IgnoresOtherArgsWhenHelpProvided) {
  const std::filesystem::path file_path = WriteRustFile("fn main() {}\n");
  const CommandResult result =
      RunRustcc({"-h", "-f", file_path.string(), "--lexer"});
  EXPECT_EQ(result.exit_code, 0);
  EXPECT_NE(result.stdout_text.find("--file"), std::string::npos);
  EXPECT_EQ(result.stderr_text, "");

  std::error_code ec;
  std::filesystem::remove(file_path, ec);
}

TEST(MainCliInput, FailsWhenNoInputIsProvided) {
  const CommandResult result = RunRustcc({});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_NE(result.stderr_text.find("--file/-f is required"), std::string::npos);
}

TEST(MainCliInput, UsesSinglePositionalAsFile) {
  const std::filesystem::path file_path = WriteRustFile("fn main() {}\n");
  const CommandResult result = RunRustcc({file_path.string()});
  EXPECT_EQ(result.exit_code, 0);

  std::error_code ec;
  std::filesystem::remove(file_path, ec);
}

TEST(MainCliInput, AcceptsOptionsAfterPositional) {
  const std::filesystem::path file_path = WriteRustFile("@\n");
  const CommandResult result = RunRustcc({file_path.string(), "--lexer"});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_NE(result.stdout_text.find("Unknown @ 1 1"), std::string::npos);

  std::error_code ec;
  std::filesystem::remove(file_path, ec);
}

TEST(MainCliInput, FailsForMultiplePositionals) {
  const CommandResult result = RunRustcc({"a.rs", "b.rs"});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_NE(result.stderr_text.find("Maximum number of positional arguments exceeded"),
            std::string::npos);
}

TEST(MainCliInput, FailsWhenFileOptionAndPositionalAreBothProvided) {
  const std::filesystem::path file_a = WriteRustFile("fn a() {}\n");
  const std::filesystem::path file_b = WriteRustFile("fn b() {}\n");
  const CommandResult result =
      RunRustcc({"-f", file_a.string(), file_b.string()});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_NE(result.stderr_text.find("Do not provide both --file/-f and a positional input file."),
            std::string::npos);

  std::error_code ec;
  std::filesystem::remove(file_a, ec);
  std::filesystem::remove(file_b, ec);
}

TEST(MainCliLexerError, ExitsNonZeroWithoutLexerOutput) {
  const std::filesystem::path file_path = WriteRustFile("@\n");
  const CommandResult result = RunRustcc({"-f", file_path.string()});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_EQ(result.stdout_text, "");
  EXPECT_NE(result.stderr_text.find("Lexer error"), std::string::npos);

  std::error_code ec;
  std::filesystem::remove(file_path, ec);
}

TEST(MainCliLexerError, PrintsTokensThenFailsWithLexerFlag) {
  const std::filesystem::path file_path = WriteRustFile("@\n");
  const CommandResult result = RunRustcc({"--lexer", "-f", file_path.string()});
  EXPECT_EQ(result.exit_code, 1);
  EXPECT_NE(result.stdout_text.find("Unknown @ 1 1"), std::string::npos);
  EXPECT_NE(result.stdout_text.find("Eof"), std::string::npos);

  std::error_code ec;
  std::filesystem::remove(file_path, ec);
}
