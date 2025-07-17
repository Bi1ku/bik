#include "../include/generator/generator.h"
#include "../include/lexer/lexer.h"
#include "../include/lexer/tokens.h"
#include "../include/parser/ast.h"
#include "../include/parser/parser.h"
#include <stdio.h>

int main() {
  TokenList *tokens = tokenize(read("../tests/test1.bik"));
  print_tokens(tokens);

  Env *env = create_env(NULL, 10);
  ProgramStmt *program = parse(tokens, env);

  generate_asm("output.asm", env, program->body);

  return 0;
}
