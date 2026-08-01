# bik

A work-in-progress compiler for a small custom language, written in C and targeting MIPS assembly.

## Overview

bik tokenizes source code, parses it into an abstract syntax tree, evaluates constant expressions, and generates MIPS assembly. I built it as a hands-on exploration of the compiler pipeline, including lexing, precedence parsing, environments, constant folding, and code generation.

The compiler is still under development, and MIPS generation is not yet complete.

## Features

* Custom lexer with identifiers, numeric and string literals, keywords, operators, and single-line comments
* AST-based parser with operator precedence and parenthesized expressions
* Integer, decimal, and string values
* Variable assignments and identifier lookup
* Arithmetic operations with compile-time constant folding
* String concatenation with automatic number conversion
* `log()` statements
* Early support for functions, typed parameters, calls, and return statements
* Basic MIPS `.data` and `.text` generation

## Tech Stack

* **C**
* **CMake**
* **MIPS assembly**

## Installation

Make sure a C compiler and CMake are installed, then clone and build the project:

```bash
git clone https://github.com/Bi1ku/bik.git
cd bik

cmake -S . -B build-local
cmake --build build-local
```

## Usage

The current entry point compiles `tests/test1.bik`. Edit that file to experiment with the language:

```text
value = 7 - 8 / 2 * 5.5;
message = "hello, world!";

log(value);
log(message);
```

Run the compiler from the build directory:

```bash
cd build-local
./execute
```

The compiler prints the generated tokens and writes the resulting assembly to `build-local/output.asm`.

Passing a source file through the command line is not yet supported.

## License

Licensed under the [MIT License](LICENSE).
