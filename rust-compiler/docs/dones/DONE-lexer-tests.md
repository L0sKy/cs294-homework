# DONE: lexer-tests

## 0) Goal achieved
- Added GTest-based lexer unit tests covering keywords, identifiers, literals, operators, punctuation, positions, EOF, and unknown tokens.
- Integrated tests into CMake with a dedicated `tests/CMakeLists.txt` and `unit` labels.
- Split the build to include a `rustcc_lib` target for test linkage.

## 1) Action items completed
- [x] Created `docs/plans/PLAN-lexer-tests.md`.
- [x] Updated root `CMakeLists.txt` to add `rustcc_lib` and include `tests/` under `BUILD_TESTING`.
- [x] Added `tests/CMakeLists.txt` with googletest FetchContent and `unit` labels.
- [x] Added lexer test suite at `tests/lexer/lexer_tests.cpp`.

## 2) Changes made
- New test targets and test discovery in `tests/CMakeLists.txt`.
- New lexer test coverage in `tests/lexer/lexer_tests.cpp`.
- Root CMake changes to support testing and library reuse in `CMakeLists.txt`.

## 3) Technical decisions and notes
- Fetched googletest in `tests/CMakeLists.txt` to keep test-only dependencies scoped to the test directory.
- Tests are written against the lexer design doc and assume `Token` exposes `type/value/line/col` accessors; if the implementation differs, tests must be aligned.
- Tests were not executed because `cmake` is not available in the environment (`cmake: command not found`).
- With the current repository state (no lexer implementation), unit tests are expected to fail until the lexer exists.

## 4) Further improvements
- Implement `include/lexer.h` and `src/lexer.cpp` to satisfy the test expectations.
- Add a minimal `main.cpp` so the `rustcc` target can link successfully.
- Add CI to run `ctest -L unit` after building with `-DBUILD_TESTING=ON`.
