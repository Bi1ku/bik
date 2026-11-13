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
* Integer functions with up to four parameters, calls, and return statements
* Integer `if` conditions and recursive function calls
* Classes with default fields, instances, and named methods
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

Pass a source file to the compiler:

```text
value = 7 - 8 / 2 * 5.5;
message = "hello, world!";

log(value);
log(message);
```

```bash
./build-local/execute program.bik -o program.asm
```

Output defaults to `output.asm`. Add `--tokens` to print the token stream, or
`--help` for usage. Running without arguments from the build directory keeps
the original `../tests/test1.bik` entry point.

Integer functions can call themselves and branch on a comparison:

```text
countdown = func(int n) {
  if (n > 0) {
    log(n);
    countdown(n - 1);
  };
};
```

The current function-call generator supports up to four integer arguments.

Classes can define fields with default values and named methods. Each call to
the class name creates an instance with its own fields. Access fields with dot
notation and call class methods using `ClassName.method()`:

```text
class Math {
  scale = 1;
  add = func(int left, int right) {
    ret left + right;
  };
};

first = Math();
first.scale = 2;
log(first.scale);
Math.add(2, 3);
```

Instances currently use default field values, and methods are class-level rather
than bound to an individual instance.

## Tests

The regression suite uses Python 3 and runs through CTest:

```bash
ctest --test-dir build-local --output-on-failure
```

You can also run it directly:

```bash
python3 tests/run.py --compiler build-local/execute -v
```

## License

Licensed under the [MIT License](LICENSE).
