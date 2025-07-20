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
  Var *items[100]; // switch to pointer later
} VarList;

struct Env {
  Env *parent;
  VarList *items;
};

Env *create_env(Env *parent, int init_size);
void add_to_env(VarList *var_list, Var *var);

Expr *get_var(VarList *items, char *key);
int get_index_of_var(VarList *items, char *key);
Var *create_var(char *key, Expr *expr, VarList *vars);

#endif
