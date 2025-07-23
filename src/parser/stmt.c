#include "../../include/parser/expr.h"
#include "../../include/parser/parser.h"
#include "../../include/parser/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NodeList *parse_func_args(TokenList *tokens) {
  NodeList *nodes = create_node_list(5);

  while (strcmp(peek(tokens).value, ")") != 0) {
    if (peek(tokens).type == IDENTIFIER)
      add_node_to_node_list(
          nodes, create_stmt_node(create_param_stmt(eat(tokens).value)));
    else
      expect(tokens, COMMA);
  }
  expect(tokens, PAREN_R);

  return nodes;
}

Node *parse_stmt(NodeList *nodes, TokenList *tokens, Env *env) {
  switch (peek(tokens).type) {
  case KEYWORD:
    if (strcmp(peek(tokens).value, "func") == 0) {
      eat(tokens);
      expect(tokens, PAREN_L);
      NodeList *params = parse_func_args(tokens);
      expect(tokens, BLOCK_START);

      NodeList *program = create_program()->program;
      while (peek(tokens).type != BLOCK_END) {
        parse_line(program, tokens, env);
        expect(tokens, NEWLINE);
      }
      expect(tokens, BLOCK_END);

      return create_stmt_node(create_func_stmt("test", params, program));
    }

    else if (strcmp(peek(tokens).value, "ret") == 0) {
      eat(tokens);
      Node *expr = parse_additive(tokens);
      return create_stmt_node(create_ret_stmt(expr->expr));
    }

    else if (strcmp(peek(tokens).value, "log") == 0) {
      eat(tokens);
      expect(tokens, PAREN_L);
      Node *node = parse_additive(tokens);

      if (node->type == STMT) {
        printf("ERROR: Expected to parse an expression but got statement "
               "instead.");
        exit(EXIT_FAILURE);
      }

      expect(tokens, PAREN_R);
      return create_stmt_node(create_log_stmt(node->expr));
    }

    else {
      exit(EXIT_FAILURE);
    }
    break;

  case IDENTIFIER:
    if (tokens->tokens[1].type == ASSIGNMENT) {
      char *name = eat(tokens).value;
      eat(tokens);

      NodeList *temp = parse_line(nodes, tokens, env);
      Node *rest = &temp->nodes[temp->size - 1];

      if (rest->type == EXPR) {
        create_var(name, rest->expr, env->items);
        // expr node must be at end after assignment
        remove_node_from_node_list(nodes, temp->size - 1);
        return create_stmt_node(create_assign_stmt(name, rest->expr));
      }

      else if (rest->stmt->type == FUNC) {
        rest->stmt->func->name = strdup(name);
        remove_node_from_node_list(nodes, temp->size - 1);
        return rest;
      }

      remove_node_from_node_list(nodes, temp->size - 1);
      return parse_additive(tokens);
    }

  default:
    return parse_additive(tokens);
  }
}
