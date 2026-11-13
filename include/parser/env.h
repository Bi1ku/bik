#ifndef ENV_H
#define ENV_H

#include "ast.h"
typedef struct Env Env;

typedef struct {
  char *key;
  Expr *value;
} Var;

typedef struct {
  int capacity;
  int size;
  Var **items;
} VarList;

struct Env {
  Env *parent;
  VarList *items;
  NodeList *classes;
};

Env *create_env(Env *parent, int init_size);
void add_to_env(VarList *var_list, Var *var);

Expr *get_var(VarList *items, char *key);
int get_index_of_var(VarList *items, char *key);
Var *create_var(char *key, Expr *expr, VarList *vars);
ClassDecl *get_class(Env *env, char *name);
void add_class(Env *env, ClassDecl *class_decl);

#endif
