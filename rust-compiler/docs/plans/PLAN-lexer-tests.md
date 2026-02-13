# Plan: Lexer Tests (GTest) + CMake Integration (With Commits)

## Goal
Create a comprehensive lexer test suite based on `docs/designs/lexer.md`, integrate it with CMake using `tests/CMakeLists.txt`, and label all tests as `unit`. Tests may initially fail until lexer implementation exists. Googletest will be fetched in `tests/CMakeLists.txt` (not in root).

## Scope
- Add GTest-based lexer unit tests.
- Split test configuration into `tests/CMakeLists.txt` and link it from root.
- Add a reusable library target `rustcc_lib` for tests to link against.
- Add a DONE summary after implementation.

## Implementation Steps (Ordered)
1. Documentation (plan-first)
   - Create this plan document and commit it.
2. CMake integration
   - Create `rustcc_lib` from `${RUSTCC_SOURCES}` and `${RUSTCC_HEADERS}`.
   - Link `rustcc` against `rustcc_lib`.
   - Keep `include(CTest)` and add `add_subdirectory(tests)` under `if(BUILD_TESTING)`.
   - Remove any `googletest` FetchContent from root.
3. Tests CMake
   - Add `tests/CMakeLists.txt` to FetchContent `googletest`.
   - Add `lexer_tests` and register with `gtest_discover_tests` labeled `unit`.
4. Lexer tests
   - Add `tests/lexer/lexer_tests.cpp` using `std::istringstream`.
   - Cover keywords, identifiers, literals, operators, punctuation, whitespace/positions, EOF, unknown/error.
5. Validate
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
   - `ctest --test-dir build -L unit`
6. Commit tests + CMake integration with pre-commit hooks enabled.
7. Add DONE summary and commit it.

## Notes
- If tests fail due to missing lexer implementation, document failures in DONE summary.
- If `Token` lacks accessors, add minimal const getters.
