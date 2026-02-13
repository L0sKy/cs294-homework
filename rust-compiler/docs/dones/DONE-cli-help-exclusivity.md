# DONE: cli-help-exclusivity

## 0) Goal achieved
- Made `--help/-h` mutually exclusive with all other arguments; it always prints help and exits 0.
- Enabled GNU-style parsing so options after a positional input file are recognized.
- Enforced input source rules: exactly one of `--file/-f` or a single positional.

## 1) Action items completed
- [x] Created `docs/plans/PLAN-cli-help-exclusivity.md`.
- [x] Updated `src/main.cpp` argument parsing and validation.
- [x] Added/updated CLI tests to cover help exclusivity and option ordering.
- [x] Ran `ctest --test-dir build -L cli` (all passed).

## 2) Changes made
- `src/main.cpp`: added early help detection, switched positional argument to `nargs(0,1)`.
- `tests/cli/cli_tests.cpp`: new help exclusivity and option-after-positional tests; updated multiple positional expectation.

## 3) Technical decisions and notes
- Help exclusivity is enforced before parsing by scanning `argv` to avoid parser errors when `--help` is mixed with other args.
- The multiple positional error message now comes from argparse (`Maximum number of positional arguments exceeded`).

## 4) Further improvements
- Consider removing duplicate `-h/--help` registration if argparse already provides it.
- Add a CLI test that ensures `--help` output includes all custom flags and descriptions.
