# DONE: cmake-build

## 0) Goal achieved
- Added a CMake build configuration that defines project name `rust-compiler`, builds `rustcc`, and pulls `argparse` and `googletest` via `FetchContent`.
- Updated architecture documentation and added the implementation plan, both in English.

## 1) Action items completed
- [x] Created `docs/plans/PLAN-cmake-build.md`.
- [x] Updated `docs/designs/architecture.md` with a Build System section.
- [x] Added root `CMakeLists.txt` with `FetchContent` for `argparse` and `googletest`.
- [x] Added source/header collection for `src/` and `include/` and `rustcc` target.
- [x] Enabled CTest (test framework integration ready).

## 2) Changes made
- New file: `CMakeLists.txt` with FetchContent-based dependency setup and target definition.
- Documentation updates:
  - `docs/plans/PLAN-cmake-build.md`
  - `docs/designs/architecture.md`

## 3) Technical decisions and notes
- Used `GLOB_RECURSE` with `CONFIGURE_DEPENDS` to auto-include sources/headers for early-stage development.
- Chose fixed tags for dependencies (`argparse v2.9`, `googletest release-1.12.1`) for reproducibility.
- Enabled `googletest` only when `BUILD_TESTING` is on to avoid unnecessary build overhead.

## 4) Further improvements
- Add a `tests/CMakeLists.txt` once test sources exist, and wire gtest targets explicitly.
- Consider switching from `GLOB_RECURSE` to explicit file lists when the codebase stabilizes.
