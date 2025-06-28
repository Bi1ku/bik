#ifndef ENV_H
#define ENV_H

typedef struct Env Env;
typedef enum { E_STRING, E_INT, E_DOUBLE } ValueTypes;

typedef struct {
  ValueTypes type;
  union {
    char *str_val;
    int int_val;
    double double_val;
  };
} E_Value;

typedef struct {
  char *key;
  E_Value value;
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

Env *create_env(Env *parent, int init_size);

E_Value create_double_value(double value);

E_Value create_int_value(int value);

E_Value create_string_value(char *value);

Item create_item(char *key, E_Value val);

void add_to_env(ItemList *items, Item item);

#endif
