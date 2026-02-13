# DONE: lexer-implementation

## 0) Goal achieved
- Implemented lexer public API and core tokenization in `include/lexer.h` and `src/lexer.cpp`.
- Implemented `src/main.cpp` with `argparse` and `--lexer` output.
- Updated lexer and main design docs to specify comment handling and output format.

## 1) Action items completed
- [x] Added lexer public interface and implementation.
- [x] Added main entry implementation using `argparse`.
- [x] Documented comment skipping, error handling, and `--lexer` output format.

## 2) Changes made
- New files:
  - `include/lexer.h`
  - `src/lexer.cpp`
  - `src/main.cpp`
  - `docs/plans/PLAN-lexer-implementation.md`
- Updated documentation:
  - `docs/designs/lexer.md`
  - `docs/designs/main.md`
- Build system fix:
  - `CMakeLists.txt` excludes `src/main.cpp` from `rustcc_lib`.
  - `tests/CMakeLists.txt` fixes lexer test source path.

## 3) Technical decisions and notes
- Lexer skips Rust-style comments (`//`, `/* */`, and doc variants) and supports nested block comments.
- Unknown or malformed tokens produce `Unknown` tokens and emit errors to `stderr`.
- Token values preserve the input lexeme, including quotes for string/char literals.

## 4) Tests
- `cmake -S . -B build -DBUILD_TESTING=ON`
- `cmake --build build`
- `ctest --test-dir build -L unit`
- Result: 7/7 unit tests passed.

## 5) Further improvements
- Add explicit handling for escape sequences in string and char literals.
- Extend tests to cover comment skipping and unterminated comment errors.
