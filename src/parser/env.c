#include "../../include/generator/eval.h"
#include <stdlib.h>
#include <string.h>

Env *create_env(Env *parent, int init_size) {
  Env *env = malloc(sizeof(Env));
  env->parent = parent;

  env->items = malloc(sizeof(VarList));

  // env->items->items = malloc(sizeof(Item) * init_size); // when changing to
  // pointer
  env->items->capacity = init_size;
  env->items->size = 0;

  return env;
}

Expr *get_var(VarList *vars, char *key) {
  for (int i = 0; i < vars->size; i++) {
    if (strcmp(key, vars->items[i]->key) == 0) {
      return vars->items[i]->value;
    }
  }

  return NULL;
}

int get_index_of_var(VarList *items, char *key) {
  for (int i = 0; i < items->size; i++) {
    if (strcmp(key, items->items[i]->key) == 0) {
      return i;
    }
  }

  return -1;
}

Var *create_var(char *key, Expr *expr, VarList *vars) {
  switch (expr->type) {
  case BIN: {
    Expr *res = eval(expr->bin, vars);
    Var *var = create_var(key, res, vars);
    add_to_env(vars, var);
    return var;
  }

  case IDENTIFIER_EX: {
    Expr *expr = get_var(vars, expr->identifier);
    Var *var = create_var(key, expr, vars);
    add_to_env(vars, var);
    return var;
  }

  default: {
    Var *var = malloc(sizeof(Var));
    var->key = key;
    var->value = expr;
    add_to_env(vars, var);
    return var;
  }
  }
}

void resize_items(VarList *items) {
  items->capacity *= 2;
  // items->items = realloc(items->items, items->capacity * sizeof(Item)); //
  //  put back in when pointer
}

void add_to_env(VarList *var_list, Var *var) {
  int check = get_index_of_var(var_list, var->key);
  if (check != -1) {
    var_list->items[check] = var;
    return;
  }

  if (var_list->size >= var_list->capacity)
    resize_items(var_list);

  var_list->items[var_list->size] = var;
  var_list->size++;
}
