# Plan: Allow Block Statements Without Semicolons

## Summary
Update the parser to allow block statements (`if/else`, `while`, and `{...}`) without
trailing semicolons, matching Rust-style syntax. Semicolons remain required for
`let`, `return`, and expression statements. Update tests and docs accordingly.

## Scope
- Update parser grammar documentation.
- Adjust parser statement list handling.
- Update unit and integration tests.
- Record the change in DONE summary.

## Implementation Steps (Ordered)
1. Documentation
   - Update `docs/designs/parser.md` to describe semicolon policy and statement list rules.
2. Tests (Unit)
   - Add/extend parser unit tests to cover block statements without semicolons.
3. Tests (Integration)
   - Update parser integration fixture(s) to include block statements without semicolons.
4. Parser Implementation
   - Update `parse_statement_list()` to allow block statements without `;`.
5. Validation
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
   - `ctest --test-dir build -L unit`
   - `ctest --test-dir build -L integration`
6. DONE Summary
   - Update `docs/dones/DONE-parser.md` with the new behavior and tests.
