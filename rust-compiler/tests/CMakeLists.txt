include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(lexer_tests
  tests/lexer/lexer_tests.cpp
)

target_link_libraries(lexer_tests PRIVATE gtest_main rustcc_lib)

target_include_directories(lexer_tests PRIVATE include)

include(GoogleTest)

gtest_discover_tests(lexer_tests PROPERTIES LABELS unit)
