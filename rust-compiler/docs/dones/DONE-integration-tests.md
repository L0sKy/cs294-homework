# DONE: integration-tests

## 0) Goal achieved
- Added integration tests that run `rustcc --lexer` against full Rust source fixtures and compare exact stdout/stderr to golden files.
- Added a new `integration_tests` GTest target with the `integration` label.

## 1) Action items completed
- [x] Created `docs/plans/PLAN-integration-tests.md`.
- [x] Added `integration_tests` to `tests/CMakeLists.txt` with `RUSTCC_BIN` and labels.
- [x] Added `tests/integration/` fixtures and golden files.
- [x] Implemented `tests/integration/integration_tests.cpp` to execute `rustcc --lexer` and compare outputs.
- [x] Ran `ctest --test-dir build -L integration` (all passed).

## 2) Changes made
- New integration test target and label in `tests/CMakeLists.txt`.
- New integration test runner in `tests/integration/integration_tests.cpp`.
- New fixtures and golden outputs in `tests/integration/`.

## 3) Technical decisions and notes
- Golden files are stored next to the integration tests to keep inputs and expected outputs colocated.
- Stderr is compared exactly using `.err` files to make lexer error reporting stable and explicit.
- EOF line/column expectations are derived from actual lexer behavior with trailing newlines.

## 4) Further improvements
- Add integration fixtures for comment skipping and unterminated block comments.
- Add a fixture that exercises multi-line strings/chars and unknown tokens after valid tokens.
- Consider deduplicating CLI runner helpers between `tests/cli` and `tests/integration`.
