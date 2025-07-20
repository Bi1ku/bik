#include "../include/generator/generator.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  TokenList *tokens = tokenize(read("../tests/test1.bik"));
  print_tokens(tokens);

  Env *env = create_env(NULL, 10);
  NodeList *program = parse(tokens, env);

  FILE *out = fopen("output.asm", "w");

  if (out == NULL) {
    printf("ERROR: Could not write to file output.asm");
    exit(EXIT_FAILURE);
  }

  generate(out, env, program);

  return 0;
}
