# Plan: Extend Parser for Let/If/Assignment and Fix Error Duplication

## Summary
Extend the recursive-descent parser to support `let` declarations (optional `mut` and optional
 type annotation), `if/else` with `else if`, and assignment expressions. Fix duplicate error
 reporting by ensuring recovery advances at least one token when synchronization does not
 consume a recovery token.

## Scope
- Update parser grammar and AST to include `LetStmt`, `IfStmt`, and `AssignExpr`.
- Update parser tests and integration fixtures to cover new syntax.
- Improve error recovery to avoid repeated errors at the same token.

## Implementation Steps (Ordered)
1. Documentation
   - Update `docs/designs/parser.md` with new grammar and AST nodes.
2. Tests (Unit)
   - Extend `tests/parser/parser_tests.cpp` with `let`, `if/else`, and assignment cases.
3. Tests (Integration)
   - Update `tests/integration/parser_ok.rs` and `.out` to include new syntax.
4. AST and Parser
   - Add `LetStmt`, `IfStmt`, `AssignExpr` to `include/ast.h` and implement dump in `src/ast.cpp`.
   - Extend parser to handle `let`, `if/else`, and assignment in `src/parser.cpp`.
   - Fix `synchronize()` to avoid repeated errors.
5. Validation
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
   - `ctest --test-dir build -L unit`
   - `ctest --test-dir build -L integration`
6. DONE Summary
   - Update `docs/dones/DONE-parser.md` with the new features and tests.

## Notes
- Assignment is right-associative and emitted as `AssignExpr '='` in the AST dump.
- `else if` is parsed as `else` followed by a nested `if` statement.
