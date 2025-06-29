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

E_Value get_item(Env *env, char *key) {
  for (int i = 0; i < env->items->size; i++) {
    if (strcmp(key, env->items->items[i].key) == 0) {
      return env->items->items[i].value;
    }
  }

  printf("Couldn't find key: %s", key);
  exit(1);
}

E_Value create_double_value(double value) {
  E_Value val;
  val.type = E_DOUBLE;
  val.double_val = value;

  return val;
}

E_Value create_int_value(int value) {
  E_Value val;
  val.type = E_DOUBLE;
  val.int_val = value;

  return val;
}

E_Value create_string_value(char *value) {
  E_Value val;
  val.type = E_DOUBLE;
  val.str_val = value;

  return val;
}

Item create_item(char *key, E_Value val) {
  Item item;
  item.key = key;
  item.value = val;

  return item;
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
