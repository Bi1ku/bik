#include "../../include/parser/env.h"
#include <stdio.h>
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

VarValue *get_var(VarList *items, char *key) {
  for (int i = 0; i < items->size; i++) {
    if (strcmp(key, items->items[i].key) == 0) {
      return &(items->items[i].value);
    }
  }

  return NULL;
}

int get_index_of_var(VarList *items, char *key) {
  for (int i = 0; i < items->size; i++) {
    if (strcmp(key, items->items[i].key) == 0) {
      return i;
    }
  }

  return -1;
}

Var create_var(char *key, VarValue val) {
  Var var;
  var.key = key;
  var.value = val;

  return var;
}

Var create_float_var(char *key, float value) {
  VarValue val;
  val.type = FLOAT;
  val.float_val = value;

  return create_var(key, val);
}

Var create_int_var(char *key, int value) {
  VarValue val;
  val.type = INT;
  val.int_val = value;

  return create_var(key, val);
}

Var create_str_var(char *key, char *value) {
  VarValue val;
  val.type = STRING;
  val.str_val = value;

  return create_var(key, val);
}

void resize_items(VarList *items) {
  items->capacity *= 2;
  // items->items = realloc(items->items, items->capacity * sizeof(Item)); //
  //  put back in when pointer
}

void add_to_env(VarList *var_list, Var var) {
  int check = get_index_of_var(var_list, var.key);
  if (check != -1) {
    var_list->items[check] = var;
    return;
  }

  if (var_list->size >= var_list->capacity)
    resize_items(var_list);

  var_list->items[var_list->size] = var;
  var_list->size++;
}
