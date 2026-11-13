#include "../include/generator/generator.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
  char *source = NULL;
  char *output = "output.asm";
  int show_tokens = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      printf("usage: %s [source.bik] [-o output.asm] [--tokens]\n", argv[0]);
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "--tokens") == 0) {
      show_tokens = 1;
    } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
      output = argv[++i];
    } else if (argv[i][0] != '-' && source == NULL) {
      source = argv[i];
    } else {
      fprintf(stderr, "ERROR: Invalid argument '%s'. Try --help.\n", argv[i]);
      return EXIT_FAILURE;
    }
  }

  if (source == NULL) {
    source = "../tests/test1.bik";
    show_tokens = 1;
  }
  struct stat source_stat, output_stat;
  if (strcmp(source, output) == 0 ||
      (stat(source, &source_stat) == 0 && stat(output, &output_stat) == 0 &&
       source_stat.st_dev == output_stat.st_dev &&
       source_stat.st_ino == output_stat.st_ino)) {
    fprintf(stderr, "ERROR: Source and output paths must differ.\n");
    return EXIT_FAILURE;
  }

  char *text = read(source);
  TokenList *tokens = tokenize(text);
  free(text);
  if (show_tokens)
    print_tokens(tokens);

  Env *env = create_env(NULL, 10);
  NodeList *program = parse(tokens, env);

  FILE *out = fopen(output, "w");

  if (out == NULL) {
    fprintf(stderr, "ERROR: Could not write to file %s\n", output);
    exit(EXIT_FAILURE);
  }

  generate(out, env, program);

  return 0;
}
