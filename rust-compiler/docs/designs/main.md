# Main

This document in detail explain the main function of the compiler.

## Introduction

main.cpp only contain one function: int main(); this function is the entry of the compiler.

main() use argparse to process arguments.

It support these args:
--file: this arg will give a file path, which is the source code of the input rust source file.
--help: this arg will output all args usage.
--lexer: this arg will output the lexer result.

When `--lexer` is provided, output tokens one per line with the format:
`TYPE value line col`, including the EOF token.

## Interface
```cpp
int main(){
    argparse::ArgumentParser args("rustcompiler");
    args.add_argument("--file"); //add all args for argparse
    ...
    if (args.get<bool>("--help")) //if has --help, output args
    ...
    std::string input_file = args.get<std::string>("--file").c_str(); //get the input file.
    lexer(input);
    std::vector<Token> tokens = lexer.tokenizer();
    ...
    if (args.get<bool>("--lexer")) //if has --lexer, output tokens
    ...
    return 0;
}
```
