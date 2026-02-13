# Main

This document in detail explain the main function of the compiler.

## Introduction

main.cpp only contain one function: int main(); this function is the entry of the compiler.

main() use argparse to process arguments.

It support these args:
- `--file` / `-f`: the input Rust source file path. This is required unless a single
  positional argument is provided.
- `--help` / `-h`: output all args usage. Default `false`.
- `--lexer`: output the lexer result. Default `false`.

Positional arguments:
- Exactly one positional argument is allowed and it is treated as the input file
  when `-f` / `--file` is not provided.
- Multiple positional arguments are an error.

When `--lexer` is provided, output tokens one per line with the format:
`TYPE value line col`, including the EOF token.

If the lexer reports a bug, `main` should terminate immediately after tokenization.
If `--lexer` is set, termination happens after output.

## Interface
```cpp
int main(){
    argparse::ArgumentParser args("rustcompiler");
    args.add_argument("-f", "--file"); //add all args for argparse
    args.add_argument("-h", "--help").default_value(false).implicit_value(true);
    args.add_argument("--lexer").default_value(false).implicit_value(true);
    ...
    if (args.get<bool>("--help")) //if has --help, output args
    ...
    std::string input_file = args.get<std::string>("--file").c_str(); //get the input file.
    lexer(input);
    std::vector<Token> tokens = lexer.tokenizer();
    ...
    if (args.get<bool>("--lexer")) //if has --lexer, output tokens
    ...
    if (lexer.had_error()) //terminate immediately after tokenization
    ...
    return 0;
}
```
