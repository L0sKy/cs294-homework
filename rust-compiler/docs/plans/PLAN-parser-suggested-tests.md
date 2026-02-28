# Plan: Complete Suggested Parser Tests + Add For/Indexing Support

## Summary
Complete all suggested parser test cases in slides by extending lexer and parser to support
`for` loops and array indexing. Add comprehensive unit and integration tests plus corner
cases, update docs, and record the change in DONE summary.

## Scope
- Add `for`/`in` keywords and `[` `]` punctuation to lexer.
- Add AST nodes `ForStmt` and `IndexExpr` and dump support.
- Extend parser with `for` and postfix indexing.
- Add tests for all suggested cases and extra corner cases.

## Implementation Steps (Ordered)
1. Documentation
   - Update `docs/designs/parser.md` for `for`/indexing grammar and AST.
   - Update `docs/designs/lexer.md` for new keywords/punctuations.
2. Tests (Unit)
   - Add suggested cases and corner cases to `tests/parser/parser_tests.cpp`.
3. Tests (Integration)
   - Update `tests/integration/parser_ok.*` and `parser_err.*` for new syntax.
4. Lexer + AST
   - Update `src/lexer.cpp`, `include/ast.h`, `src/ast.cpp`.
5. Parser
   - Add `parse_for_stmt`, `parse_postfix`, and indexing support.
6. Validation
   - Build and run unit/integration tests.
7. DONE Summary
   - Update `docs/dones/DONE-parser.md` with new features and tests.
