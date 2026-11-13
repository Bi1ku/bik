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
* Arithmetic operations with compile-time constant folding, unary signs, and integer remainder (`%`)
* String concatenation with automatic number conversion
* `log()` statements
* Integer functions with up to four parameters, nested expression calls, and return values
* Integer `if` conditions and recursive function calls
* Classes with default fields, independent instances, single inheritance, and method overrides
* Basic MIPS `.data` and `.text` generation

## Tech Stack

* **C**
* **CMake**
* **MIPS assembly**

## Installation

Make sure a C compiler, CMake 3.12+, and Python 3 are installed, then clone and
build the project. SPIM is needed to run the generated assembly.

```bash
git clone https://github.com/Bi1ku/bik.git
cd bik

cmake -S . -B build-local
cmake --build build-local
```

## Demo

The character demo includes a `Character → Warrior → Paladin` hierarchy and a
`Mage` branch. It uses inherited defaults, overridden damage methods, calls
through instances, and independent health values:

```bash
./build-local/execute tests/programs/inheritance.bik -o build-local/demo.asm
spim -file build-local/demo.asm
```

```text
paladin: ready for battle
health: 180, armor: 10
enemy after paladin strike: 63
enemy after mage strike: 36
independent health: 75 / 180
mage mana: 80
```

There is also a recursive factorial, Fibonacci, and greatest-common-divisor demo:

```bash
./build-local/execute tests/programs/recursion.bik -o build-local/recursion.asm
spim -file build-local/recursion.asm
```

It prints `5040 144 21`.

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

Calls can appear inside arithmetic, conditions, arguments, assignments, and
`log()`. Arguments and return values are integers, with up to four arguments.
Functions can call functions declared later, including mutual recursion.
Falling off the end of a function returns zero.

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

Use `extends` to inherit from a previously declared class. A child can replace
field defaults and methods; further descendants inherit those replacements:

```text
class ScaledMath extends Math {
  scale = 3;
  add = func(int left, int right) {
    ret (left + right) * 3;
  };
};

calculator = ScaledMath();
log(calculator.scale);
log(calculator.add(2, 3));
```

`instance.method()` selects the method from that instance's class. Methods still
use explicit arguments: there is no implicit `self`, `super`, or constructor
body yet. Integer assignments and field updates run in statement order. Strings
and decimals retain compile-time initialization; function-local variable scopes
and runtime updates to non-integer values are still unfinished. The original
`tests/test1.bik` is an older syntax sample with unsupported non-integer calls;
use the demos above for executable examples.

## Tests

The regression suite checks inheritance, overrides, recursive algorithms, argument
order, all six comparisons, deep expressions, large inputs, and invalid programs.
It uses Python 3 and runs through CTest:

```bash
ctest --test-dir build-local --output-on-failure
```

Install SPIM to execute the generated MIPS and compare its output. Without
SPIM, runtime checks are skipped. Require it when running the full suite:

```bash
python3 tests/run.py --compiler build-local/execute --require-spim -v
```

## License

Licensed under the [MIT License](LICENSE).
