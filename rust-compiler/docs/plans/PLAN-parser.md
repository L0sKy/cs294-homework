# Plan: Parser Support + AST Dump + Tests-First (No Lexer Changes)

## Summary
Implement a recursive-descent parser using a strict tests-first workflow. All parser tests
(unit + integration) are created and fixed before any parser or `--parser` code. No
lexer/token changes are required. AST dump is implemented as subclass methods and called
recursively from the root. Design docs are updated before any code changes.

## Files to Create / Update

### Design Docs (Step 1)
- `docs/designs/parser.md`
  - Grammar subset (no `for`, no indexing)
  - AST node definitions
  - Parser API (`peek/match/expect`)
  - Error handling & recovery
  - AST dump contract (subclass `dump()`, clang-style prefix)
- `docs/designs/main.md`
  - Add `--parser` description and output format (clang-like tree)

### Tests (Steps 2–3)
- `tests/parser/parser_tests.cpp`
- `tests/CMakeLists.txt` (add `parser_tests` target with label `unit`)
- `tests/integration/parser_ok.rs`
- `tests/integration/parser_ok.out`
- `tests/integration/parser_ok.err`
- `tests/integration/parser_err.rs`
- `tests/integration/parser_err.out`
- `tests/integration/parser_err.err`
- `tests/integration/integration_tests.cpp` (add parser integration tests)

### Implementation (Steps 4–5)
- `include/ast.h`
- `src/ast.cpp`
- `include/parser.h`
- `src/parser.cpp`
- `src/main.cpp` (add `--parser`)

### Summary
- `docs/dones/DONE-parser.md`

## Step 0: Planning Commit
1. Create `docs/plans/PLAN-parser.md` with this plan.
2. Commit the plan.

## Step 1: Design Docs First
3. Add `docs/designs/parser.md`.
4. Update `docs/designs/main.md` for `--parser`.
5. Commit docs.

## Step 2: All Parser Unit Tests Before Implementation
6. Add `tests/parser/parser_tests.cpp`.
7. Add `parser_tests` target in `tests/CMakeLists.txt`.
8. Run unit tests (expected fail), commit tests with `--no-verify`.

## Step 3: All Parser Integration Tests Before Implementation
9. Add parser fixtures and update `tests/integration/integration_tests.cpp`.
10. Run integration tests (expected fail), commit with `--no-verify`.

## Step 4: Implement AST Dump (Only)
11. Implement `include/ast.h` + `src/ast.cpp` with subclass `dump()` methods.
12. Ensure clang-style prefix and indentation rules.
13. Commit AST dump implementation.

## Step 5: Implement Parser + `--parser`
14. Implement `include/parser.h` + `src/parser.cpp` (recursive descent).
15. Update `src/main.cpp` to add `--parser`, calling `root->dump(...)`.
16. Run unit + integration tests until green. Commit parser + CLI changes.

## Step 6: DONE Summary
17. Add `docs/dones/DONE-parser.md` and commit after pre-commit checks.
