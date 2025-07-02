#include "../include/generator/eval.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/env.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  // TODO: Better error handling for lexer
  TokenList *tokens =
      tokenize("x = 10 + 1; y = x * 2; z = y + (7 - 2) * 2; y = x - x;");
  print_tokens(tokens);

  ProgramStmt *program = parse(tokens, NULL);

  return 0;
}
