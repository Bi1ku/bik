#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Env *create_env(Env *parent, int init_size) {
  Env *env = malloc(sizeof(Env));
  env->parent = parent;

  env->items = malloc(sizeof(ItemList));

  // env->items->items = malloc(sizeof(Item) * init_size); // when changing to
  // pointer
  env->items->capacity = init_size;
  env->items->size = 0;

  return env;
}

Var *get_var(ItemList *items, char *key) {
  for (int i = 0; i < items->size; i++) {
    if (strcmp(key, items->items[i].key) == 0) {
      return &(items->items[i].value);
    }
  }

  return NULL;
}

int get_index_of_var(ItemList *items, char *key) {
  for (int i = 0; i < items->size; i++) {
    if (strcmp(key, items->items[i].key) == 0) {
      return i;
    }
  }

  return -1;
}

Item create_item(char *key, Var val) {
  Item item;
  item.key = key;
  item.value = val;

  return item;
}

Item create_float_var(char *key, float value) {
  Var val;
  val.type = FLOAT;
  val.float_val = value;

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
  int check = get_index_of_var(items, item.key);
  if (check != -1) {
    items->items[check] = item;
    return;
  }

  if (items->size >= items->capacity)
    resize_items(items);

  items->items[items->size] = item;
  items->size++;
}
