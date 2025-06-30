#include "../include/generator/eval.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/env.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  TokenList *tokens = tokenize("x = 10 + 1; y = x * 2; z = y + (7 - 2) * 2;");
  print_tokens(tokens);

  Env *env = create_env(NULL, 10);

  ProgramStmt *program = parse(tokens, env);
  eat_token(tokens);
  program = parse(tokens, env);
  eat_token(tokens);
  program = parse(tokens, env);

  return 0;
}
