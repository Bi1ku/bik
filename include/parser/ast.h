#ifndef AST_H
#define AST_H

typedef enum { EXPR, STMT } NodeType;
typedef enum {
  PROGRAM_STMT,
  ASSIGN_STMT,
  FUNC_STMT,
  PARAM_STMT,
  RET_STMT
} StmtType;
typedef enum {
  INT_EXPR,
  FLOAT_EXPR,
  IDENTIFIER_EXPR,
  BIN_EXPR,
  STRING_EXPR
} ExprType;

typedef struct Node Node;
typedef struct NodeList NodeList;

typedef struct Expr Expr;
typedef struct BinaryExpr BinaryExpr;
typedef struct IdentifierExpr IdentifierExpr;
typedef struct IntExpr IntExpr;
typedef struct FloatExpr FloatExpr;
typedef struct StringExpr StringExpr;

typedef struct Stmt Stmt;
typedef struct ProgramStmt ProgramStmt;
typedef struct FuncStmt FuncStmt;
typedef struct ParamStmt ParamStmt;
typedef struct AssignStmt AssignStmt;
typedef struct RetStmt RetStmt;

typedef struct NodeList NodeList;
struct Node {
  NodeType type;
  union {
    Stmt *stmt;
    Expr *expr;
  };
};

struct Stmt {
  StmtType type;
  union {
    RetStmt *retStmt;
    ProgramStmt *programStmt;
    AssignStmt *assignStmt;
    FuncStmt *funcStmt;
    ParamStmt *paramStmt;
  };
};

struct Expr {
  ExprType type;
  union {
    BinaryExpr *bin_expr;
    IdentifierExpr *identifier_expr;
    IntExpr *int_expr;
    FloatExpr *float_expr;
    StringExpr *string_expr;
  };
};

struct NodeList {
  int capacity;
  int size;
  Node
      nodes[100]; // simply to visualize in debug mode [CHANGE LATER] to pointer
};

struct RetStmt {
  Expr *expr;
};

struct ParamStmt {
  char *symbol;
};

struct FuncStmt {
  char *name;
  NodeList *params;
  NodeList *body;
};

struct ProgramStmt {
  NodeList *body;
};

struct AssignStmt {
  char *symbol;
  Expr *expr;
};

struct BinaryExpr {
  Expr *left;
  Expr *right;
  char *op;
};

struct IdentifierExpr {
  char *symbol;
};

struct IntExpr {
  int value;
};

struct FloatExpr {
  float value;
};

struct StringExpr {
  char *value;
};

NodeList *create_node_list(int init_size);

void remove_node_from_node_list(NodeList *nodes, int index);

void add_node_to_node_list(NodeList *nodes, Node *node);

Node *create_stmt_node(Stmt *stmt);

Node *create_expr_node(Expr *expr);

Stmt *create_assign_stmt(char *symbol, Expr *expr);

Stmt *create_program();

Expr *create_bin_expr(Expr *left, Expr *right, char *op);

Expr *create_int_expr(int value);

Expr *create_float_expr(float value);

Expr *create_identifier_expr(char *symbol);

Expr *create_string_expr(char *value);

Stmt *create_param_stmt(char *symbol);

Stmt *create_func_stmt(char *name, NodeList *params, NodeList *body);

Stmt *create_ret_stmt(Expr *expr);

#endif
