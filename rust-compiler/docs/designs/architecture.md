# Architecture of rust-compiler

This document describes the architecture of rust-compiler.
This porject aims only to implement front-end for rust language and generate LLVM-IR for input. 
Different modules will add sequentially. 
For lexer module details, see [lexer.md](./lexer.md).

## Language and Compile Management

For generating LLVM IRs and easily connected with LLVM back-end, the whole project will be implemented by using CPP language.
For easily managing and testing whole prject, CMAKE will be used as building system.

## Build System

The build system uses CMake. The project name is `rust-compiler`, and the main executable is `rustcc`.
Dependencies are fetched with CMake `FetchContent`, including `argparse` and `googletest` (test-only).
All headers under `include/` and sources under `src/` are included in the `rustcc` target.
