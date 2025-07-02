#include "../include/generator/eval.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/env.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  TokenList *tokens =
      tokenize("x = \"Hello, \" + (1 + 5) + \" World!\"; y = x + 1;");
  print_tokens(tokens);

  ProgramStmt *program = parse(tokens, NULL);

  return 0;
}
