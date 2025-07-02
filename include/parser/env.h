#ifndef ENV_H
#define ENV_H

typedef struct Env Env;
typedef enum { STRING, INT, DOUBLE } VarTypes;

typedef struct {
  VarTypes type;
  union {
    char *str_val;
    int int_val;
    double double_val;
  };
} Var;

typedef struct {
  char *key;
  Var value;
} Item;

typedef struct {
  int capacity;
  int size;
  Item items[100]; // for now, switch to pointer later
} ItemList;

struct Env {
  Env *parent;
  ItemList *items;
};

Var *get_var(ItemList *items, char *key);

Env *create_env(Env *parent, int init_size);

Item create_double_var(char *key, double value);

Item create_int_var(char *key, int val);

Item create_str_var(char *key, char *val);

void add_to_env(ItemList *items, Item item);

#endif
