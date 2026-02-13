# Plan: CMake Build System for rust-compiler

## Goal
Create a top-level CMake build configuration that:
- Uses `FetchContent` to pull `argparse` and `googletest`.
- Sets the project name to `rust-compiler`.
- Builds an executable named `rustcc`.
- Includes all headers under `include/` and all sources under `src/`.

All documents must be written in English.

## Scope
- Add a new `CMakeLists.txt` at the repository root.
- Update architecture documentation to describe the build system.
- Add a DONE summary document after implementation.

## Implementation Steps (Ordered)
1. Update documentation first.
   - Create this plan document.
   - Update `docs/designs/architecture.md` with a Build System section.
2. Implement build system.
   - Create root `CMakeLists.txt`.
   - Use CMake `FetchContent` to pull:
     - `p-ranav/argparse` at tag `v2.9`.
     - `google/googletest` at tag `release-1.12.1` (test-only).
   - Collect sources and headers via `GLOB_RECURSE` and add executable `rustcc`.
   - Set C++ standard to C++17.
   - Enable CTest and gate googletest usage behind `BUILD_TESTING`.
3. Validate build configuration.
   - `cmake -S . -B build`
   - `cmake --build build`
   - `ctest --test-dir build`
4. Produce a DONE summary in `dones/DONE-cmake-build.md`.

## Notes
- `GLOB_RECURSE` is acceptable for the current project size.
- If `src/` is empty, the executable will not link; add sources later.
