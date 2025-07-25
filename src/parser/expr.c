#include "../../include/lexer/tokens.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include "../../include/parser/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_additive(TokenList *tokens, char *mem, VarList *vars);

Node *parse_values(TokenList *tokens, char *mem, VarList *vars) {
  switch (peek(tokens).type) {
  case NUM: {
    char *value = eat(tokens).value;

    if (strchr(value, '.') != NULL) {
      Expr *expr = create_float_expr(atof(value), mem);
      if (mem != NULL)
        create_var(mem, expr, vars);
      return create_expr_node(expr);
    }

    return create_expr_node(create_int_expr(atoi(value)));
  }

  case STR: {
    Expr *expr = create_string_expr(eat(tokens).value, mem);
    if (mem != NULL)
      create_var(mem, expr, vars);
    return create_expr_node(expr);
  }

  case IDENTIFIER:
    return create_expr_node(create_identifier_expr(eat(tokens).value));

  case PAREN_L:
    eat(tokens);
    Node *sub_expr = parse_additive(tokens, mem, vars);
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

Node *parse_multiplicative(TokenList *tokens, char *mem, VarList *vars) {
  Node *left = parse_values(tokens, mem, vars);

  while (strcmp(peek(tokens).value, "*") == 0 ||
         strcmp(peek(tokens).value, "/") == 0 ||
         strcmp(peek(tokens).value, "%") == 0) {
    char *op = eat(tokens).value;
    Node *right = parse_values(tokens, mem, vars);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}

Node *parse_additive(TokenList *tokens, char *mem, VarList *vars) {
  Node *left = parse_multiplicative(tokens, mem, vars);

  while (strcmp(peek(tokens).value, "+") == 0 ||
         strcmp(peek(tokens).value, "-") == 0) {
    char *op = eat(tokens).value;
    Node *right = parse_multiplicative(tokens, mem, vars);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}
