# DONE: parser

## 0) Goal achieved
- Implemented recursive-descent parser and AST dump with clang-like tree output.
- Added parser unit tests and integration tests, including error-case fixtures.
- Added `--parser` CLI flag to print AST dump and exit non-zero on parser errors.

## 1) Action items completed
- [x] Added parser design documentation.
- [x] Wrote parser unit tests before implementation.
- [x] Wrote parser integration tests before implementation.
- [x] Implemented AST dump as subclass methods.
- [x] Implemented parser and `--parser` CLI support.
- [x] Ran unit and integration tests.

## 2) Changes made
- New AST and parser implementation:
  - `include/ast.h`
  - `src/ast.cpp`
  - `include/parser.h`
  - `src/parser.cpp`
- CLI updates:
  - `src/main.cpp` adds `--parser` handling.
- Documentation:
  - `docs/designs/parser.md`
  - `docs/designs/main.md`
- Tests:
  - `tests/parser/parser_tests.cpp`
  - `tests/integration/parser_ok.*`, `tests/integration/parser_err.*`
  - `tests/integration/integration_tests.cpp`
  - `tests/CMakeLists.txt` adds `parser_tests`
- Repo hygiene:
  - `.gitignore` updated to ignore reference docs and `.DS_Store`.

## 3) Technical decisions and notes
- AST dump uses virtual `dump()` on subclasses and a shared prefix helper to match
  clang-style tree connectors (`|-`, ``-`) with increasing indentation.
- Parser error recovery syncs on `;`, `}`, or `fn` to avoid infinite loops while
  keeping later declarations parsable.
- `--parser` always dumps the current AST (even when errors exist) and then exits
  non-zero if parser errors were recorded.

## 4) Tests
- `cmake -S . -B build -DBUILD_TESTING=ON`
- `cmake --build build`
- `ctest --test-dir build -L unit`
- `ctest --test-dir build -L integration`
