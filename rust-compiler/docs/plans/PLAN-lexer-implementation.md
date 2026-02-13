# Plan: Implement Lexer Module + Main Entry (With Comment Skipping)

## Summary
Implement the lexer module and main entry point according to `docs/designs/lexer.md` and
`docs/designs/main.md`, and align behavior with existing GTest cases in
`tests/lexer/lexer_tests.cpp`. Extend lexer to skip Rust-style comments (line, block, and
Doc comments). Update design docs to record comment handling, `--lexer` output format, and
error handling. Run lexer tests until they pass.

## Scope
- Add public lexer interface in `include/lexer.h`.
- Implement lexer in `src/lexer.cpp` with comment skipping.
- Implement main entry point in `src/main.cpp` using `argparse`.
- Update design docs for comment behavior, output format, and error handling.
- Run tests and iterate until all lexer tests pass.

## Decisions
- `--lexer` output: one token per line in a simple text format.
- Error handling: emit `Unknown` tokens and continue tokenization while also reporting
  errors to `stderr`.
- Comment handling: skip all Rust-style comments (including doc comments), do not produce
  tokens for comments.

## Comment Rules
- Line comments: start with `//`, skip through end of line.
  - Doc comments: `///` and `//!` are still treated as line comments and skipped.
- Block comments: start with `/*`, end at the matching `*/`.
  - Doc comments: `/**` and `/*!` are still treated as block comments and skipped.
  - Nested block comments are supported.
- Unterminated block comment: emit `Unknown` with the consumed text, print error to
  `stderr`, then continue to EOF.

## File-by-File Changes
1. `docs/designs/lexer.md`
   - Add a Comments section specifying supported comment forms, skipping behavior, nested
     comments, and unterminated block comment error handling.
   - Add/keep Error Handling section.
   - Clarify `Token` getters for type/value/line/col.

2. `docs/designs/main.md`
   - Specify `--lexer` output format: one token per line as `TYPE value line col`, including
     EOF.

3. `include/lexer.h`
   - Public API:
     - `enum class TokenType` with all required variants.
     - `class Token` with const getters.
     - `class Lexer` with `Lexer(std::istream&)` and `tokenize()`.

4. `src/lexer.cpp`
   - Implement tokenization with line/col tracking, whitespace skipping, comment skipping,
     keywords/identifiers, literals, operators, punctuation, unknown tokens, and EOF token.

5. `src/main.cpp`
   - Implement `main` using `argparse` with `--file`, `--lexer`, `--help`.
   - Print tokens as `TYPE value line col` when `--lexer` is set.

## Tests and Validation
1. Configure and build:
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
2. Run lexer tests:
   - `ctest --test-dir build -L unit`
3. Fix any failures and rerun until all pass.

## DONE Summary
- Add `docs/dones/DONE-lexer-implementation.md` with achievements, action items, changes,
  and technical decisions.
