import argparse
from pathlib import Path
import subprocess
import tempfile
import unittest


parser = argparse.ArgumentParser()
parser.add_argument("--compiler", required=True, type=Path)
options, remaining = parser.parse_known_args()
COMPILER = options.compiler.resolve()


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


if __name__ == "__main__":
    unittest.main(argv=[__file__, *remaining])
