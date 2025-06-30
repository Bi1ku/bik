#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Env *create_env(Env *parent, int init_size) {
  Env *env = malloc(sizeof(Env));
  env->parent = parent;

  env->items = malloc(sizeof(ItemList));
  env->items->capacity = init_size;
  env->items->size = 0;

  return env;
}

Var get_var(Env *env, char *key) {
  for (int i = 0; i < env->items->size; i++) {
    if (strcmp(key, env->items->items[i].key) == 0) {
      return env->items->items[i].value;
    }
  }

  // fix
  printf("Couldn't find key: %s", key);
  exit(1);
}

Item create_item(char *key, Var val) {
  Item item;
  item.key = key;
  item.value = val;

  return item;
}

Item create_double_var(char *key, double value) {
  Var val;
  val.type = DOUBLE;
  val.double_val = value;

  return create_item(key, val);
}

Item create_int_var(char *key, int val) {
  Var var;
  var.type = INT;
  var.int_val = val;

  return create_item(key, var);
}

Item create_str_var(char *key, char *val) {
  Var var;
  var.type = STRING;
  var.str_val = val;

  return create_item(key, var);
}

void resize_items(ItemList *items) {
  items->capacity *= 2;
  // items->items = realloc(items->items, items->capacity * sizeof(Item)); //
  //  put back in when pointer
}

void add_to_env(ItemList *items, Item item) {
  if (items->size >= items->capacity)
    resize_items(items);

  items->items[items->size] = item;
  items->size++;
}
