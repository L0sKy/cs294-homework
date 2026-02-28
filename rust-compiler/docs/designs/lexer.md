# Lexer

This document in detail explain the lexer module of the compiler.

## Simple Introduction

The lexer accept a rust source file as input, and output some tokens if all tokens are legal, otherwise, report error in terminal.
Token types which should be handled will be added sequentially.

## Token types which should be handled

### Keywords

Keywords are reserved words for rust. for now, the lexer should support:
fn, let, mut, if, else, while, for, in, return

### Identifier
Identifiers are names of functions or variables, which must start with [a-zA-Z_].

### Literal
lexer should support:
Number, such as 42, 0, et al.
String, such as "yes", "hello world", et al.
Char, such as 'a', '中', et al.
Boolean, such as true, false.

### Operator
lexer should support: 
```
+ , - , * , / , = , == , != , < , > , <= , >=
```

### Punctuation
lexer should support: 
```
( , ) , { , } , [ , ] , ; , : , ,
```

### Comments
Lexer should skip all Rust-style comments and should NOT emit tokens for them.
Supported comment forms:
- Line comment: starts with `//`, ends at newline or EOF.
  - Doc comments `///` and `//!` are treated the same as line comments.
- Block comment: starts with `/*`, ends at the matching `*/`.
  - Doc comments `/**` and `/*!` are treated the same as block comments.
  - Nested block comments are supported.

If a block comment is unterminated, the lexer should emit an `Unknown` token
containing the consumed comment text, report error in terminal, and continue
to EOF.

### Error Handling
If a token is not supported or malformed (e.g. unterminated string/char),
the lexer should emit an `Unknown` token with the consumed text and report
error in terminal. The lexer should continue tokenization afterward.

The lexer should expose an error query API (for example `bool had_error() const`)
that returns true if any error was reported during tokenization. `main` uses
this to decide whether to exit with a non-zero status after tokenization.

## Data structure

This section will tell the data structures which will be used.

### Lexer
```cpp
class Lexer {
    public:
        Lexer(std::istream& input); // file descriptor
        std::vector<Token> tokenize(); // core function to get tokens
        bool had_error() const; // whether any error was reported
    private:
        bool hasNext(); // more tokens?
        Token nextToken(); // get next token
};
```
this class Lexer is the core class of the lexer, it will use tokenize to peek all chars to get the right tokens. Each token will be put in the class Token object.

### Token
```cpp
class Token {
    private:
        TokenType type; // which type?
        std::string value; // value of the token
        size_t line; // which line the token located
        size_t col; // which col the token located
};
```
This class Token is the fundamation class of the lexer, it will create object for all tokens. the TokenType is a enum, which must cover token types above, and should divide Literals into 4 types. It also need to contain EOF and Unknown, EOF means the end of the file. Unknown means the token is not support and should report error.

Token should expose const getters for `type`, `value`, `line`, and `col`.
