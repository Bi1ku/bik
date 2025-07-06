#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  TokenList *tokens = tokenize(read("../tests/test1.bik"));
  print_tokens(tokens);

  ProgramStmt *program = parse(tokens, NULL);

  return 0;
}
