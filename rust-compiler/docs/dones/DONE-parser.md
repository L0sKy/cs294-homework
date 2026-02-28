# DONE: parser

## 0) Goal achieved
- Implemented recursive-descent parser and AST dump with clang-like tree output.
- Added parser unit tests and integration tests, including error-case fixtures.
- Added `--parser` CLI flag to print AST dump and exit non-zero on parser errors.
- Extended parser with `let`, `if/else` (including `else if`), and assignment support.
- Added a follow-up plan document for parser extensions.

## 1) Action items completed
- [x] Added parser design documentation.
- [x] Wrote parser unit tests before implementation.
- [x] Wrote parser integration tests before implementation.
- [x] Implemented AST dump as subclass methods.
- [x] Implemented parser and `--parser` CLI support.
- [x] Ran unit and integration tests.
- [x] Added `let`, `if/else`, and assignment parsing and tests.

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
- Extended AST and parser:
  - `LetStmt`, `IfStmt`, and `AssignExpr` AST nodes.
  - Assignment parsing and `else if` handling.
- Added plan document:
  - `docs/plans/PLAN-parser-extensions.md`

## 3) Technical decisions and notes
- AST dump uses virtual `dump()` on subclasses and a shared prefix helper to match
  clang-style tree connectors (`|-`, ``-`) with increasing indentation.
- Parser error recovery syncs on `;`, `}`, or `fn` to avoid infinite loops while
  keeping later declarations parsable.
- `--parser` always dumps the current AST (even when errors exist) and then exits
  non-zero if parser errors were recorded.
- Assignment is parsed as a right-associative expression node and emitted as
  `AssignExpr '='` in the dump for clarity.

## 4) Tests
- `cmake -S . -B build -DBUILD_TESTING=ON`
- `cmake --build build`
- `ctest --test-dir build -L unit`
- `ctest --test-dir build -L integration`
