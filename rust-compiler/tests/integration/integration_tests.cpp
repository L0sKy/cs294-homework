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
  const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
  return std::filesystem::temp_directory_path() /
         (name_prefix + "-" + std::to_string(now) + suffix);
}

CommandResult RunRustcc(const std::vector<std::string>& args) {
  const std::filesystem::path stdout_path = MakeTempPath("rustcc-stdout", ".txt");
  const std::filesystem::path stderr_path = MakeTempPath("rustcc-stderr", ".txt");

  std::ostringstream command;
  command << '"' << RUSTCC_BIN << '"';
  for (const auto& arg : args) {
    command << " \"" << arg << '\"';
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

std::filesystem::path FixturePath(const std::string& filename) {
  return std::filesystem::path(__FILE__).parent_path() / filename;
}

}  // namespace

TEST(IntegrationLexer, FullProgramOutputMatchesGolden) {
  const std::filesystem::path input_path = FixturePath("full_program.rs");
  const std::filesystem::path expected_out = FixturePath("full_program.out");
  const std::filesystem::path expected_err = FixturePath("full_program.err");

  const CommandResult result = RunRustcc({"--lexer", "-f", input_path.string()});
  EXPECT_EQ(result.exit_code, 0);
  EXPECT_EQ(result.stdout_text, ReadFile(expected_out));
  EXPECT_EQ(result.stderr_text, ReadFile(expected_err));
}

TEST(IntegrationLexer, UnknownTokenMatchesGolden) {
  const std::filesystem::path input_path = FixturePath("unknown.rs");
  const std::filesystem::path expected_out = FixturePath("unknown.out");
  const std::filesystem::path expected_err = FixturePath("unknown.err");

  const CommandResult result = RunRustcc({"--lexer", "-f", input_path.string()});
  EXPECT_NE(result.exit_code, 0);
  EXPECT_EQ(result.stdout_text, ReadFile(expected_out));
  EXPECT_EQ(result.stderr_text, ReadFile(expected_err));
}
