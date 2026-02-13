# Plan: Integration Tests for rustcc CLI Lexer Output

## Goal
Add integration tests that run the `rustcc` executable against full Rust source fixtures and compare stdout/stderr to golden files. Cover a full program tokenization and an unknown-token error case.

## Scope
- Add a new `integration_tests` GTest target and `integration` label.
- Add `tests/integration/` fixtures (`.rs`, `.out`, `.err`).
- Implement integration tests that execute `rustcc --lexer` and compare exact outputs.
- Add a DONE summary after implementation.

## Implementation Steps (Ordered)
1. Documentation (plan-first)
   - Create this plan document and commit it.
2. CMake integration
   - Add `integration_tests` target in `tests/CMakeLists.txt`.
   - Link with `gtest_main`, define `RUSTCC_BIN`, and add dependency on `rustcc`.
   - Register tests with label `integration`.
3. Integration test fixtures
   - Add `tests/integration/full_program.rs`.
   - Add `tests/integration/full_program.out` and `tests/integration/full_program.err` (empty).
   - Add `tests/integration/unknown.rs`.
   - Add `tests/integration/unknown.out` and `tests/integration/unknown.err`.
4. Integration test code
   - Add `tests/integration/integration_tests.cpp` to run `rustcc --lexer` on fixtures.
   - Assert exact stdout and stderr matches for each fixture.
   - Assert exit code is 0 for full program and non-zero for unknown case.
5. Validate
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
   - `ctest --test-dir build -L integration`
6. Commit with pre-commit checks enabled.
7. Add DONE summary and commit it.

## Notes
- Golden files are stored next to the integration tests under `tests/integration/`.
- Stderr is compared exactly using `.err` files.
