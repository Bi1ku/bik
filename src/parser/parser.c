#include "../../include/lexer/tokens.h"
#include "../../include/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token peek(TokenList *tokens) { return (tokens->tokens)[0]; }

Token eat_token(TokenList *tokens) {
  Token token = peek(tokens);

  for (int i = 1; i < tokens->size; i++) {
    tokens->tokens[i - 1] = tokens->tokens[i];
  }
  tokens->size--;

  return token;
}

Node *parse_additive(TokenList *tokens);

// parse actual values like 5, foo(), x, 3.5
Node *parse_values(TokenList *tokens) {
  switch (peek(tokens).type) {
  case INT:
    return create_expr_node(create_int_expr(atoi(eat_token(tokens).value)));

  case DOUBLE:
    return create_expr_node(create_double_expr(atof(eat_token(tokens).value)));

  case IDENTIFIER:
    return create_expr_node(create_identifier_expr(eat_token(tokens).value));

  case PAREN_L:
    eat_token(tokens);
    Node *sub_expr = parse_additive(tokens);
    if (peek(tokens).type == PAREN_R)
      eat_token(tokens);
    else {
      printf("Expected ')'");
      exit(0);
    }
    return sub_expr;

  default:
    printf("UNEXPECTED TOKEN: %s", peek(tokens).value);
    exit(1);
  }
}

Node *parse_multiplicative(TokenList *tokens) {
  Node *left = parse_values(tokens);

  while (strcmp(peek(tokens).value, "*") == 0 ||
         strcmp(peek(tokens).value, "/") == 0 ||
         strcmp(peek(tokens).value, "%") == 0) {
    char *op = eat_token(tokens).value;
    Node *right = parse_values(tokens);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}

Node *parse_additive(TokenList *tokens) {
  Node *left = parse_multiplicative(tokens);

  while (strcmp(peek(tokens).value, "+") == 0 ||
         strcmp(peek(tokens).value, "-") == 0) {
    char *op = eat_token(tokens).value;
    Node *right = parse_multiplicative(tokens);
    left = create_expr_node(create_bin_expr(left->expr, right->expr, op));
  }

  return left;
}

ProgramStmt *parse(TokenList *tokens) {
  Stmt *program = create_program();

  while (peek(tokens).type != NEWLINE) {
    add_node_to_node_list(program->programStmt->body, parse_additive(tokens));
  }

  return program->programStmt;
}
