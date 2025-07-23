#include "../../include/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>

// NODE LIST DYNAMIC ARRAY
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
  stmt->type = ASSIGN;

  Assign *assign = malloc(sizeof(Assign));
  assign->symbol = symbol;
  assign->expr = expr;

  stmt->assign = assign;

  return stmt;
}

Stmt *create_ret_stmt(Expr *expr) {
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = RET;
  stmt->ret = expr;
  return stmt;
}

Stmt *create_func_decl_stmt(char *name, NodeList *params, NodeList *body) {
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = FUNC_DECL;

  stmt->func_decl = malloc(sizeof(FuncDecl));
  stmt->func_decl->name = name;
  stmt->func_decl->params = params;
  stmt->func_decl->body = body;

  return stmt;
}

Stmt *create_func_call_stmt(char *name, NodeList *args) {
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = FUNC_CALL;

  stmt->func_call = malloc(sizeof(FuncCall));
  stmt->func_call->name = name;
  stmt->func_call->args = args;

  return stmt;
}

Stmt *create_log_stmt(Expr *expr) {
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = LOG;
  stmt->log = expr;
  return stmt;
}

Stmt *create_param_stmt(char *symbol, ExprType type) {
  Stmt *stmt = malloc(sizeof(Stmt));

  stmt->type = PARAM;
  stmt->param = malloc(sizeof(Param));
  stmt->param->name = symbol;
  stmt->param->type = type;

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
  stmt->type = PROGRAM;
  stmt->program = create_node_list(10);
  return stmt;
}

Expr *create_bin_expr(Expr *left, Expr *right, char *op) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = BIN;

  expr->bin = malloc(sizeof(Bin));
  expr->bin->left = left;
  expr->bin->right = right;
  expr->bin->op = op;

  return expr;
}

Expr *create_int_expr(int value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = INT;
  expr->integer = value;
  return expr;
}

Expr *create_float_expr(float value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = FLOAT;
  expr->floating = value;
  return expr;
}

Expr *create_identifier_expr(char *symbol) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = IDENTIFIER_EX;
  expr->identifier = symbol;
  return expr;
}

Expr *create_string_expr(char *value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = STRING;
  expr->str = value;
  return expr;
}
