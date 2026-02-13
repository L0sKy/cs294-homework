include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(lexer_tests
  lexer/lexer_tests.cpp
)

target_link_libraries(lexer_tests PRIVATE gtest_main rustcc_lib)

target_include_directories(lexer_tests PRIVATE include)

add_executable(cli_tests
  cli/cli_tests.cpp
)

target_link_libraries(cli_tests PRIVATE gtest_main)
target_compile_definitions(cli_tests PRIVATE RUSTCC_BIN="$<TARGET_FILE:rustcc>")
add_dependencies(cli_tests rustcc)

add_executable(integration_tests
  integration/integration_tests.cpp
)

target_link_libraries(integration_tests PRIVATE gtest_main)
target_compile_definitions(integration_tests PRIVATE RUSTCC_BIN="$<TARGET_FILE:rustcc>")
add_dependencies(integration_tests rustcc)

include(GoogleTest)

gtest_discover_tests(lexer_tests PROPERTIES LABELS unit)
gtest_discover_tests(cli_tests PROPERTIES LABELS cli)
gtest_discover_tests(integration_tests PROPERTIES LABELS integration)
