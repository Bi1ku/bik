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


if __name__ == "__main__":
    unittest.main(argv=[__file__, *remaining])
