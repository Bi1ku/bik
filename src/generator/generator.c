#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_asm(char *filename, Env *env) {
  FILE *file;

  file = fopen(filename, "w");
  if (file == NULL) {
    printf("ERROR: Could not open file %s for writing\n", filename);
    exit(EXIT_FAILURE);
  }

  fprintf(file, ".data\n");
  for (int i = 0; i < env->items->size; i++) {
    Var value = env->items->items[i].value;
    char *name = env->items->items[i].key;

    if (name != NULL) {
      char *line = malloc(sizeof(char) * 100);

      switch (value.type) {
      case STRING:
        snprintf(buffer, sizeof(buffer), "%s", value.str_val);
        fprintf(file, buffer);
        break;

      case INT: {
        char buffer[4];
        sprintf(buffer, "%d", value.int_val);

        fprintf(file, strcat(strcat(name, ": .half"), buffer));
        break;
      }

      case DOUBLE: {
        char buffer[4];
        sprintf(buffer, "%f", value.double_val);

        fprintf(file, strcat(strcat(name, ": .half"), buffer));
      }
      }
    }
  }

  fclose(file);
}
