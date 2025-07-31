#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>

void generate_vars(FILE *out, Env *env) {
  for (int i = 0; i < env->items->size; i++) {
    Expr *value = env->items->items[i]->value;
    char *name = env->items->items[i]->key;

    if (name != NULL) {
      switch (value->type) {
      case STRING:
        fprintf(out, "\t%s: .asciiz \"%s\"\n", name, value->str->val);
        break;

      case INT:
        fprintf(out, "\t%s: .word %d\n", name, value->integer);
        break;

      case FLOAT:
        fprintf(out, "\t%s: .float %lf\n", name, value->floating->val);
        break;

      default:
        printf("ERROR: Not Valid!");
        exit(EXIT_FAILURE);
      }
    }
  }
}
