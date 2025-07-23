#include "../../include/parser/stmt.h"
#include "../../include/parser/utils.h"
#include <stdio.h>

NodeList *parse_line(NodeList *nodes, TokenList *tokens, Env *env) {
  while (peek(tokens).type != NEWLINE) {
    add_node_to_node_list(nodes, parse_stmt(nodes, tokens, env));
  }

  return nodes;
}

NodeList *parse(TokenList *tokens, Env *env) {
  NodeList *program = create_program()->program;

  if (env == NULL)
    env = create_env(NULL, 10);

  while (tokens->tokens[0].type != END) {
    parse_line(program, tokens, env);
    eat(tokens);
  }

  return program;
}
