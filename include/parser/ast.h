#ifndef AST_H
#define AST_H

typedef enum { EXPR, STMT } NodeType;

typedef enum {
  PROGRAM,
  ASSIGN,
  FUNC_DECL,
  PARAM,
  RET,
  LOG,
  FUNC_CALL,
  IF,
  CLASS_DECL
} StmtType;
typedef enum { INT, FLOAT, STRING, BIN, IDENTIFIER_EX, OBJECT, CALL } ExprType;

typedef struct NodeList NodeList;
typedef struct Expr Expr;

typedef struct {
  char *val;
  char *mem;
} Str;

typedef struct {
  float val;
  char *mem;
} Float;

typedef struct {
  char *name;
  ExprType type;
} Param;

typedef struct {
  char *name;
  NodeList *params;
  NodeList *body;
} FuncDecl;

typedef struct {
  char *name;
  NodeList *args;
} FuncCall;

typedef struct {
  char *symbol;
  Expr *expr;
} Assign;

typedef struct {
  Expr *left;
  Expr *right;
  char *op;
  NodeList *body;
} IfStmt;

typedef struct {
  char *name;
  NodeList *fields;
  NodeList *methods;
} ClassDecl;

typedef struct {
  ClassDecl *class_decl;
} ObjectValue;

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
    Float *floating;
    Str *str;
    ObjectValue *object;
    FuncCall *call;
  };
};

typedef struct {
  StmtType type;
  union {
    Expr *ret;
    NodeList *program;
    Assign *assign;
    FuncDecl *func_decl;
    FuncCall *func_call;
    Param *param;
    Expr *log;
    IfStmt *if_stmt;
    ClassDecl *class_decl;
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
  Node *nodes;
};

NodeList *create_node_list(int init_size);
void remove_node_from_node_list(NodeList *nodes, int index);
void add_node_to_node_list(NodeList *nodes, Node *node);

Node *create_stmt_node(Stmt *stmt);
Node *create_expr_node(Expr *expr);

Expr *create_bin_expr(Expr *left, Expr *right, char *op);
Expr *create_int_expr(int value);
Expr *create_float_expr(float value, char *mem);
Expr *create_string_expr(char *value, char *mem);
Expr *create_identifier_expr(char *symbol);

Stmt *create_func_decl_stmt(char *name, NodeList *params, NodeList *body);
Stmt *create_func_call_stmt(char *name, NodeList *args);
Stmt *create_param_stmt(char *symbol, ExprType type);
Stmt *create_assign_stmt(char *symbol, Expr *expr);
Stmt *create_ret_stmt(Expr *expr);
Stmt *create_log_stmt(Expr *expr);
Stmt *create_if_stmt(Expr *left, char *op, Expr *right, NodeList *body);
Stmt *create_class_decl_stmt(char *name, NodeList *fields, NodeList *methods);
Expr *create_object_expr(ClassDecl *class_decl);
Expr *create_call_expr(char *name, NodeList *args);
Stmt *create_program();

#endif
