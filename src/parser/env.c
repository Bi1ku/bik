#include "../../include/generator/eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Env *create_env(Env *parent, int init_size) {
  Env *env = malloc(sizeof(Env));
  env->parent = parent;

  env->items = malloc(sizeof(VarList));
  env->classes = create_node_list(5);

  env->items->items = malloc(sizeof(Var *) * init_size);
  env->items->capacity = init_size;
  env->items->size = 0;

  return env;
}

ClassDecl *get_class(Env *env, char *name) {
  for (int i = 0; i < env->classes->size; i++) {
    Stmt *stmt = env->classes->nodes[i].stmt;
    if (stmt->type == CLASS_DECL && strcmp(stmt->class_decl->name, name) == 0)
      return stmt->class_decl;
  }
  return env->parent == NULL ? NULL : get_class(env->parent, name);
}

void add_class(Env *env, ClassDecl *class_decl) {
  for (int i = 0; i < env->classes->size; i++) {
    Stmt *stmt = env->classes->nodes[i].stmt;
    if (stmt->type == CLASS_DECL &&
        strcmp(stmt->class_decl->name, class_decl->name) == 0) {
      stmt->class_decl = class_decl;
      return;
    }
  }
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = CLASS_DECL;
  stmt->class_decl = class_decl;
  add_node_to_node_list(env->classes, create_stmt_node(stmt));
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
    return var;
  }

  case IDENTIFIER_EX: {
    Expr *value = get_var(vars, expr->identifier);
    if (value == NULL) {
      printf("ERROR: Unknown variable '%s'.\n", expr->identifier);
      exit(EXIT_FAILURE);
    }
    return create_var(key, value, vars);
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
  items->items = realloc(items->items, items->capacity * sizeof(Var *));
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
