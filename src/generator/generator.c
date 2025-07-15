#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_asm(char *filename, Env *env) {
  FILE *file;

  file = fopen("output.asm", "w");
  if (file == NULL) {
    printf("ERROR: Could not open file %s for writing\n", filename);
    exit(EXIT_FAILURE);
  }

  fprintf(file, ".data\n");

  for (int i = 0; i < env->items->size; i++) {
    Var value = env->items->items[i].value;
    char *name = env->items->items[i].key;

    if (name != NULL) {
      switch (value.type) {
      case STRING:
        fprintf(file, "%s: .asciz \"%s\"\n", name, value.str_val);
        break;

      case INT:
        fprintf(file, "%s: .word %d\n", name, value.int_val);
        break;

      case DOUBLE:
        fprintf(file, "%s: .word %lf\n", name, value.double_val);
        break;
      }
    }
  }

  fprintf(file, "\n.text\n");

  fclose(file);
}
