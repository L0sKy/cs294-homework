# Plan: CLI Help Exclusivity + GNU-Style Options

## Goal
Make `--help/-h` mutually exclusive with all other arguments (if present, print help and exit 0), allow options after positionals (GNU-style), and enforce exactly one input source: either `--file/-f` or a single positional.

## Scope
- Update `src/main.cpp` argument parsing and validation.
- Update CLI tests to cover help exclusivity and option ordering.
- Add DONE summary after implementation.

## Implementation Steps (Ordered)
1. Documentation (plan-first)
   - Create this plan document and commit it.
2. CLI parsing changes
   - Replace `args.add_argument("input").remaining();` with `args.add_argument("input").nargs(0, 1);`.
   - Add an early `--help` check on raw `argv` to short-circuit and print help when present.
   - Enforce input source rules: exactly one input source (`--file/-f` xor single positional), otherwise error.
3. Tests
   - Add cases to `tests/cli/cli_tests.cpp`:
     - `-h` with `--file` or positional should still print help and exit 0.
     - Options after positional are accepted (e.g., `file.rs --lexer`).
     - Mutual exclusivity between `--file` and positional remains enforced.
     - No-input error remains.
4. Validate
   - `cmake -S . -B build -DBUILD_TESTING=ON`
   - `cmake --build build`
   - `ctest --test-dir build -L cli`
5. Commit with pre-commit checks enabled.
6. Add DONE summary and commit it.

## Notes
- `--help` should never error due to other args; it only prints help and exits 0.
- CLI flags remain unchanged; only parsing behavior changes.
