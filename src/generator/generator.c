#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_asm(char *filename, Env *env, NodeList *program) {
  FILE *file;

  file = fopen("output.asm", "w");
  if (file == NULL) {
    printf("ERROR: Could not open file %s for writing\n", filename);
    exit(EXIT_FAILURE);
  }

  fprintf(file, "section .data\n");

  for (int i = 0; i < env->items->size; i++) {
    VarValue value = env->items->items[i].value;
    char *name = env->items->items[i].key;

    if (name != NULL) {
      switch (value.type) {
      case STRING:
        fprintf(file, "\t%s: .asciz \"%s\"\n", name, value.str_val);
        break;

      case INT:
        fprintf(file, "\t%s: .word %d\n", name, value.int_val);
        break;

      case FLOAT:
        fprintf(file, "\t%s: .word %lf\n", name, value.float_val);
        break;
      }
    }
  }

  fprintf(file, "\nsection .text\n");

  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.stmt->type == FUNC_STMT) {
      FuncStmt *func = node.stmt->funcStmt;
      fprintf(file, "%s:\n", func->name);

      for (int i = 0; i < func->body->size; i++) {
        // ..
      }
    }
  }

  fclose(file);
}
