#ifndef AST_H
#define AST_H

typedef enum { EXPR, STMT } NodeType;

typedef enum { PROGRAM, ASSIGN, FUNC, PARAM, RET } StmtType;
typedef enum { INT, FLOAT, STRING, BIN, IDENTIFIER_EX } ExprType;

typedef struct NodeList NodeList;
typedef struct Expr Expr;

typedef struct {
  char *name;
  NodeList *params;
  NodeList *body;
} Func;

typedef struct {
  char *symbol;
  Expr *expr;
} Assign;

typedef struct {
  Expr *left;
  Expr *right;
  char *op;
} Bin;

struct Expr {
  ExprType type;
  union {
    Bin *bin;
    char *identifier;
    int integer;
    float floating;
    char *str;
  };
};

typedef struct {
  StmtType type;
  union {
    Expr *ret;
    NodeList *program;
    Assign *assign;
    Func *func;
    char *param;
  };
} Stmt;

typedef struct {
  NodeType type;
  union {
    Stmt *stmt;
    Expr *expr;
  };
} Node;

struct NodeList {
  int capacity;
  int size;
  Node nodes[100]; // change to pointer later
};

NodeList *create_node_list(int init_size);
void remove_node_from_node_list(NodeList *nodes, int index);
void add_node_to_node_list(NodeList *nodes, Node *node);

Node *create_stmt_node(Stmt *stmt);
Node *create_expr_node(Expr *expr);

Expr *create_bin_expr(Expr *left, Expr *right, char *op);
Expr *create_int_expr(int value);
Expr *create_float_expr(float value);
Expr *create_identifier_expr(char *symbol);
Expr *create_string_expr(char *value);

Stmt *create_assign_stmt(char *symbol, Expr *expr);
Stmt *create_program();
Stmt *create_param_stmt(char *symbol);
Stmt *create_func_stmt(char *name, NodeList *params, NodeList *body);
Stmt *create_ret_stmt(Expr *expr);

#endif
