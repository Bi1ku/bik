#include "../include/generator/eval.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  TokenList *tokens = tokenize("x = 10;");
  print_tokens(tokens);

  ProgramStmt *program = parse(tokens); // use debugger to see

  return 0;
}
