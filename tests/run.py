import argparse
from pathlib import Path
import subprocess
import shutil
import tempfile
import unittest


parser = argparse.ArgumentParser()
parser.add_argument("--compiler", required=True, type=Path)
parser.add_argument("--require-spim", action="store_true")
options, remaining = parser.parse_known_args()
COMPILER = options.compiler.resolve()
SPIM = shutil.which("spim")
if options.require_spim and SPIM is None:
    parser.error("spim is required but was not found")


class CompilerTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.source = self.root / "program.bik"
        self.output = self.root / "program.asm"

    def invoke(self, *args):
        return subprocess.run(
            [str(COMPILER), *map(str, args)], cwd=self.root,
            capture_output=True, text=True, timeout=10,
        )

    def compile(self, source, *args):
        self.source.write_text(source)
        result = self.invoke(self.source, "-o", self.output, *args)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return self.output.read_text(), result

    def run_program(self, source, expected):
        self.compile(source)
        if SPIM is None:
            self.skipTest("install spim to check program output")
        result = subprocess.run(
            [SPIM, "-file", str(self.output)], capture_output=True,
            text=True, timeout=10,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stderr, "")
        self.assertIn("Loaded:", result.stdout)
        actual = result.stdout.split("Loaded:", 1)[1].split("\n", 1)[1]
        self.assertEqual(actual, expected)

    def reject(self, source, message):
        self.source.write_text(source)
        result = self.invoke(self.source, "-o", self.output)
        self.assertGreater(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn(message, result.stdout + result.stderr)

    def test_help(self):
        result = self.invoke("--help")
        self.assertEqual(result.returncode, 0)
        self.assertIn("usage:", result.stdout)
        self.assertFalse((self.root / "output.asm").exists())

    def test_bad_options(self):
        for args in [("--wat",), ("-o",), ("one.bik", "two.bik")]:
            with self.subTest(args=args):
                result = self.invoke(*args)
                self.assertGreater(result.returncode, 0)
                self.assertIn("Invalid argument", result.stderr)

    def test_missing_source(self):
        result = self.invoke("missing.bik")
        self.assertGreater(result.returncode, 0)
        self.assertIn("Cannot open file", result.stdout + result.stderr)

    def test_source_is_not_output(self):
        self.source.write_text("log(42);")
        result = self.invoke(self.source, "-o", self.source)
        self.assertGreater(result.returncode, 0)
        self.assertEqual(self.source.read_text(), "log(42);")

    def test_output_error(self):
        self.source.write_text("log(42);")
        result = self.invoke(self.source, "-o", self.root / "missing" / "out.asm")
        self.assertGreater(result.returncode, 0)
        self.assertIn("Could not write", result.stderr)

    def test_output_alias(self):
        self.source.write_text("log(42);")
        self.output.symlink_to(self.source)
        result = self.invoke(self.source, "-o", self.output)
        self.assertGreater(result.returncode, 0)
        self.assertEqual(self.source.read_text(), "log(42);")

    def test_arithmetic(self):
        assembly, result = self.compile("answer = (7 + 5) * 3; log(answer);")
        self.assertIn("answer: .word 36", assembly)
        self.assertIn("main:", assembly)
        self.assertEqual(result.stdout, "")

    def test_tokens(self):
        _, result = self.compile("log(42);", "--tokens")
        self.assertIn("NUM: 42", result.stdout)

    def test_default_output(self):
        self.source.write_text("log(42);")
        result = self.invoke(self.source)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertTrue((self.root / "output.asm").is_file())

    def test_class_instances(self):
        assembly, _ = self.compile("""
class Counter {
  value = 1;
};
first = Counter();
second = Counter();
first.value = 7;
log(first.value);
log(second.value);
""")
        self.assertIn("first_value: .word 7", assembly)
        self.assertIn("second_value: .word 1", assembly)

    def test_compact_syntax(self):
        self.run_program("value=7;log(value*6);if(value>=7){log(1);};", "421")

    def test_signed_remainder(self):
        self.run_program("a=-17%5;log(a);log(17%-5);log(+(2+3)*-4);", "-22-20")

    def test_long_strings(self):
        left, right = "a" * 180, "b" * 240
        self.run_program(f'message="{left}"+123+"{right}";log(message);',
                         left + "123" + right)

    def test_numeric_string_conversion(self):
        self.run_program('a=12+" apples";b=" pi="+3.25;log(a);log(b);',
                         "12 apples pi=3.250000")

    def test_large_program(self):
        source = "".join(f"v{i}={i};" for i in range(180))
        source += "log(v0);log(v99);log(v179);"
        self.run_program(source, "099179")

    def test_long_identifier(self):
        name = "value_" + "x" * 160
        self.run_program(f"{name}=42;log({name});", "42")

    def test_windows_whitespace_and_comments(self):
        self.run_program("// comment\r\nx=6;\r\n\tlog(x*7);// eof", "42")

    def test_invalid_sources(self):
        cases = [
            ('log("unfinished);', "Unterminated string"),
            ("x=1.2.3;", "Invalid number"),
            ("log(1@2);", "Unexpected character"),
            ("if(1!2){log(0);};", "Expected '='"),
            ("x=4/0;", "Division by zero"),
            ("x=4%0;", "Division by zero"),
            ("x=4.5%2;", "Remainder requires integers"),
            ("x=unknown+1;", "Unknown variable"),
            ("log((1+2);", "Expected token"),
            ("log(1)", "Unexpected token"),
        ]
        for source, message in cases:
            with self.subTest(source=source):
                self.reject(source, message)

    def test_recursive_algorithms(self):
        source = Path(__file__).with_name("programs") / "recursion.bik"
        self.run_program(source.read_text(), "5040 144 21\n")

    def test_inheritance_demo(self):
        source = Path(__file__).with_name("programs") / "inheritance.bik"
        self.run_program(source.read_text(),
                         "paladin: ready for battle\n"
                         "health: 180, armor: 10\n"
                         "enemy after paladin strike: 63\n"
                         "enemy after mage strike: 36\n"
                         "independent health: 75 / 180\n"
                         "mage mana: 80\n")

    def test_nested_calls_and_argument_order(self):
        self.run_program("""
identity = func(int n) { ret n; };
pack = func(int a, int b, int c, int d) {
  ret a * 1000 + b * 100 + c * 10 + d;
};
rotate = func(int a, int b, int c, int d) {
  log(d);
  ret pack(b, a, identity(d), identity(c));
};
log(rotate(1, 2, 3, 4));
log(pack(identity(1), identity(2), identity(3), identity(4)));
""", "421431234")

    def test_calls_in_conditions_and_assignments(self):
        self.run_program("""
twice = func(int n) { ret n * 2; };
x = twice(3);
y = 24 / x;
if (twice(x) == twice(y + 2)) { log(x + y); };
x = twice(y);
log(x);
""", "108")

    def test_mutual_recursion(self):
        self.run_program("""
even = func(int n) {
  if (n == 0) { ret 1; };
  ret odd(n - 1);
};
odd = func(int n) {
  if (n == 0) { ret 0; };
  ret even(n - 1);
};
log(even(20));
log(odd(20));
log(odd(21));
""", "101")

    def test_deep_expression(self):
        expression = "1"
        for _ in range(40):
            expression = f"1+({expression})"
        self.run_program(f"log({expression});", "41")

    def test_all_comparisons(self):
        for op, true_pair, false_pair in [
            ("==", (2, 2), (2, 3)), ("!=", (2, 3), (2, 2)),
            ("<", (-1, 2), (2, 2)), (">", (3, 2), (2, 2)),
            ("<=", (2, 2), (3, 2)), (">=", (2, 2), (1, 2)),
        ]:
            with self.subTest(op=op):
                a, b = true_pair
                c, d = false_pair
                self.run_program(f"if({a}{op}{b}){{log(1);}};"
                                 f"if({c}{op}{d}){{log(0);}};", "1")

    def test_inherited_overrides_leave_parent_unchanged(self):
        self.run_program("""
class Base {
  value = 3;
  score = func(int n) { ret n + 1; };
};
class Child extends Base {
  value = 8;
  score = func(int n) { ret n * 2; };
};
class Leaf extends Child {};
base = Base(); child = Child(); leaf = Leaf();
log(base.value); log(child.value); log(leaf.value);
log(base.score(3)); log(child.score(3)); log(leaf.score(3));
log(Base.score(3)); log(Child.score(3));
""", "38846646")

    def test_inherited_decimal_defaults(self):
        self.run_program("""
class Base { ratio = 1.5 + 0.5; };
class Child extends Base {};
child = Child();
log(child.ratio);
""", "2.00000000")

    def test_integer_updates_follow_statement_order(self):
        self.run_program("""
x = 1; log(x);
x = x + 2; log(x);
if (x > 9) { x = 99; };
log(x);
class Counter { count = 10; };
c = Counter(); log(c.count);
c.count = c.count - 3; log(c.count);
c = Counter(); log(c.count);
""", "13310710")

    def test_call_and_inheritance_errors(self):
        cases = [
            ("log(missing(1));", "Unknown function"),
            ("f=func(int n){ret n;};f();", "expects 1 arguments"),
            ("f=func(){ret 1;};f(1,2,3,4,5);", "at most four arguments"),
            ("f=func(int a,int b,int c,int d,int e){ret a;};", "four parameters"),
            ("f=func(deci n){ret n;};", "parameters must be integers"),
            ("f=func(int n){ret n;};f(1.5);", "Expected an integer"),
            ("f=func(int n,int n){ret n;};", "Duplicate parameter"),
            ("f=func(){ret 1;};f=func(){ret 2;};", "Duplicate function"),
            ("ret 1;", "Return outside"),
            ("log(unknown);", "Unknown variable"),
            ("f=func(int n){ret n;};f(1,);", "Unexpected token"),
            ("f=func(int n){ret n;};f(1 2);", "Expected token"),
            ("class Child extends Missing {};", "Invalid parent"),
            ("class Self extends Self {};", "Invalid parent"),
            ("class A {};class A {};", "Duplicate class"),
            ("class A {};a=A(1);", "constructors do not take arguments"),
            ("f=func(){ret 2;};class A {value=f();};", "must be constant"),
        ]
        for source, message in cases:
            with self.subTest(source=source):
                self.reject(source, message)


if __name__ == "__main__":
    unittest.main(argv=[__file__, *remaining])
