#include "../../include/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>

NodeList *create_node_list(int init_size) {
  NodeList *nodes = malloc(sizeof(NodeList));
  nodes->size = 0;
  nodes->capacity = init_size;
  // nodes->nodes = malloc(init_size * sizeof(Node));

  return nodes;
}

void resize_node_list(NodeList *nodes) {
  nodes->capacity *= 2;
  // nodes->nodes = realloc(nodes->nodes, nodes->capacity * sizeof(Node));
  // simply to visualize in debug mode [CHANGE LATER] to pointer
}

void add_node_to_node_list(NodeList *nodes, Node *node) {
  if (nodes->size >= nodes->capacity)
    resize_node_list(nodes);

  nodes->nodes[nodes->size] = *node;
  nodes->size++;
}

Node *create_stmt_node(Stmt *stmt) {
  Node *node = malloc(sizeof(Node));
  node->type = STMT;
  node->stmt = stmt;

  return node;
}

void remove_node_from_node_list(NodeList *nodes, int index) {
  if (index < 0 || index >= nodes->size) {
    printf("ERROR: Index out of bounds\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < nodes->size - 1; i++) {
    nodes->nodes[i] = nodes->nodes[i + 1];
  }
  nodes->size--;
}

Stmt *create_assign_stmt(char *symbol, Expr *expr) { // work on this
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = ASSIGN_STMT;

  AssignStmt *assign = malloc(sizeof(AssignStmt));
  assign->symbol = symbol;
  assign->expr = expr;

  stmt->assignStmt = assign;

  return stmt;
}

Node *create_expr_node(Expr *expr) {
  Node *node = malloc(sizeof(Node));
  node->type = EXPR;
  node->expr = expr;

  return node;
}

Stmt *create_program() {
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = PROGRAM_STMT;

  ProgramStmt *program = malloc(sizeof(ProgramStmt));
  program->body = create_node_list(10);

  stmt->programStmt = program;

  return stmt;
}

Expr *create_bin_expr(Expr *left, Expr *right, char *op) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = BIN_EXPR;

  expr->bin_expr = malloc(sizeof(BinaryExpr));
  expr->bin_expr->left = left;
  expr->bin_expr->right = right;
  expr->bin_expr->op = op;

  return expr;
}

Expr *create_int_expr(int value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = INT_EXPR;

  expr->int_expr = malloc(sizeof(IntExpr));
  expr->int_expr->value = value;

  return expr;
}

Expr *create_double_expr(double value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = DOUBLE_EXPR;

  expr->double_expr = malloc(sizeof(DoubleExpr));
  expr->double_expr->value = value;

  return expr;
}

Expr *create_identifier_expr(char *symbol) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = IDENTIFIER_EXPR;

  expr->identifier_expr = malloc(sizeof(IdentifierExpr));
  expr->identifier_expr->symbol = symbol;

  return expr;
}

Expr *create_string_expr(char *value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = STRING_EXPR;

  expr->string_expr = malloc(sizeof(StringExpr));
  expr->string_expr->value = value;

  return expr;
}
