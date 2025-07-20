#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int t_reg = 0;
int f_reg = 0;

void generate_expr(FILE *out, Expr *expr, Env *env) {
  switch (expr->type) {
  case INT:
    fprintf(out, "li $t%d, %d", t_reg, expr->integer);
    t_reg++;
    break;

  case FLOAT:
    fprintf(out, "l.d $f%d, %f", f_reg, expr->floating);
    f_reg++;
    break;
  }
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
    // ...
  }
}

void generate_vars(FILE *out, Env *env) {
  for (int i = 0; i < env->items->size; i++) {
    Expr *value = env->items->items[i]->value;
    char *name = env->items->items[i]->key;

    if (name != NULL) {
      switch (value->type) {
      case STRING:
        fprintf(out, "\t%s: .asciz \"%s\"\n", name, value->str);
        break;

      case INT:
        fprintf(out, "\t%s: .word %d\n", name, value->integer);
        break;

      case FLOAT:
        fprintf(out, "\t%s: .word %lf\n", name, value->floating);
        break;

      default:
        printf("ERROR: Not Valid!");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void generate(char *filename, Env *env, NodeList *program) {
  FILE *out = fopen("output.asm", "w");

  if (out == NULL) {
    printf("ERROR: Could not open file %s for writing\n", filename);
    exit(EXIT_FAILURE);
  }

  fprintf(out, "section .data\n");
  generate_vars(out, env);

  fprintf(out, "\nsection .text\n");
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.type == EXPR)
      generate_expr(out, node.expr, env);
    else
      generate_stmt(out, node.stmt, env);
  }

  fclose(out);
}
