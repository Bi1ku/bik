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

Node *parse_additive(TokenList *tokens);

// parse actual values like 5, foo(), x, 3.5
// when we see an assignment
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
    exit(1);
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

Node *parse_stmt(NodeList *nodes, TokenList *tokens, Env *env) {
  switch (peek(tokens).type) {
  case IDENTIFIER:
    if (tokens->tokens[1].type == ASSIGNMENT) {
      char *name = eat(tokens).value;
      eat(tokens);

      NodeList *temp = parse_line(nodes, tokens, env);
      Expr *expr = temp->nodes[temp->size - 1].expr;

      if (expr->type == BIN_EXPR) {
        Var val = eval(expr->bin_expr, env);

        switch (val.type) {
        case INT:
          add_to_env(env->items, create_int_var(name, val.int_val));
          break;

        case FLOAT:
          add_to_env(env->items, create_float_var(name, val.float_val));
          break;

        case STRING:
          add_to_env(env->items, create_str_var(name, val.str_val));
          break;

        default:
          printf("ERROR: Cannot assign value of type %d to variable %s\n",
                 val.type, name);
          exit(EXIT_FAILURE);
        }
      } else if (expr->type == INT_EXPR) {
        add_to_env(env->items, create_int_var(name, expr->int_expr->value));
      } else if (expr->type == FLOAT_EXPR) {
        add_to_env(env->items, create_float_var(name, expr->float_expr->value));
      } else if (expr->type == STRING_EXPR) {
        add_to_env(env->items, create_str_var(name, expr->string_expr->value));
      } else {
        printf("ERROR: Cannot assign value of type %d to variable %s\n",
               expr->type, name);
        exit(EXIT_FAILURE);
      }

      remove_node_from_node_list(temp, temp->size -
                                           1); // remove bin_expr added before
      return create_stmt_node(create_assign_stmt(name, expr));
    }

    return parse_additive(tokens);

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

ProgramStmt *parse(TokenList *tokens, Env *env) {
  ProgramStmt *program = create_program()->programStmt;

  if (env == NULL)
    env = create_env(NULL, 10);

  while (tokens->tokens[0].type != END) {
    parse_line(program->body, tokens, env);
    eat(tokens);
  }

  return program;
}
