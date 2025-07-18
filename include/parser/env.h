#ifndef ENV_H
#define ENV_H

#include "ast.h"
typedef struct Env Env;

typedef struct {
  DataType type;
  union {
    char *str_val;
    int int_val;
    float float_val;
  };
} VarValue;

typedef struct {
  char *key;
  VarValue value;
} Var;

typedef struct {
  int capacity;
  int size;
  Var items[100]; // switch to pointer later
} VarList;

struct Env {
  Env *parent;
  VarList *items;
};

Env *create_env(Env *parent, int init_size);

VarValue *get_var(VarList *items, char *key);

int get_index_of_var(VarList *items, char *key);

Var create_var(char *key, VarValue val);

Var create_float_var(char *key, float value);

Var create_int_var(char *key, int value);

Var create_str_var(char *key, char *value);

void add_to_env(VarList *var_list, Var var);

#endif
