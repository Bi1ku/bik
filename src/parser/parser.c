#include "../../include/generator/eval.h"
#include "../../include/lexer/tokens.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NodeList *parse_line(NodeList *nodes, TokenList *tokens, Env *env);

Token peek(TokenList *tokens) { return (tokens->tokens)[0]; }

Token eat(TokenList *tokens) {
  Token token = peek(tokens);

  for (int i = 1; i < tokens->size; i++) {
    tokens->tokens[i - 1] = tokens->tokens[i];
  }
  tokens->size--;

  return token;
}

Token expect(TokenList *tokens, TokenType type) {
  Token token = eat(tokens);
  if (token.type != type) {
    printf("ERROR: Expected token of type %d but got %d\n", type, token.type);
    exit(EXIT_FAILURE);
  }
  return token;
}

Node *parse_additive(TokenList *tokens);

Node *parse_values(TokenList *tokens) {
  switch (peek(tokens).type) {
  case NUM: {
    char *value = eat(tokens).value;

    if (strchr(value, '.') != NULL)
      return create_expr_node(create_float_expr(atof(value)));

    return create_expr_node(create_int_expr(atoi(value)));
  }

  case STR:
    return create_expr_node(create_string_expr(eat(tokens).value));

  case IDENTIFIER:
    return create_expr_node(create_identifier_expr(eat(tokens).value));

  case PAREN_L:
    eat(tokens);
    Node *sub_expr = parse_additive(tokens);
    if (peek(tokens).type == PAREN_R)
      eat(tokens);
    else {
      printf("Expected ')'");
      exit(0);
    }
    return sub_expr;

  default:
    printf("ERROR: Unexpected token \"%s\"", peek(tokens).value);
    exit(EXIT_FAILURE);
  }
}

Node *parse_multiplicative(TokenList *tokens) {
  Node *left = parse_values(tokens);

  while (strcmp(peek(tokens).value, "*") == 0 ||
         strcmp(peek(tokens).value, "/") == 0 ||
         strcmp(peek(tokens).value, "%") == 0) {
    char *op = eat(tokens).value;
    Node *right = parse_values(tokens);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}

Node *parse_additive(TokenList *tokens) {
  Node *left = parse_multiplicative(tokens);

  while (strcmp(peek(tokens).value, "+") == 0 ||
         strcmp(peek(tokens).value, "-") == 0) {
    char *op = eat(tokens).value;
    Node *right = parse_multiplicative(tokens);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}

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

    } else if (strcmp(peek(tokens).value, "ret") == 0) {
      eat(tokens);
      Node *expr = parse_additive(tokens);
      return create_stmt_node(create_ret_stmt(expr->expr));
    } else {
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
