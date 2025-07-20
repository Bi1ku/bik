#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int t_reg = 0;
int f_reg = 0;
int a_reg = 0;

void generate_from_nodes(FILE *out, Env *env, NodeList *program);

void generate_expr(FILE *out, Expr *expr, Env *env) {
  switch (expr->type) {
  case INT:
    fprintf(out, "\tli $t%d, %d\n", t_reg, expr->integer);
    t_reg++;
    break;

  case FLOAT:
    fprintf(out, "\tl.d $f%d, %f\n", f_reg, expr->floating);
    f_reg++;
    break;

  case IDENTIFIER_EX: {
    Expr *var = get_var(env->items, expr->identifier);
    generate_expr(out, var, env);
    break;
  }

  case STRING:
    printf("ERROR: Haven't done string asm logic yet");
    exit(EXIT_FAILURE);

  default:
    printf("ERROR: Unsure how to generate asm with expr type %d", expr->type);
    exit(EXIT_FAILURE);
  }
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
  case FUNC:
    fprintf(out, "%s:\n", stmt->func->name);
    generate_from_nodes(out, env, stmt->func->body);
    break;

  case RET:
    generate_expr(out, stmt->ret, env);
    fprintf(out, "\tjr $ra\n");
    break;

  case ASSIGN: // already handled in generate_vars
    break;

  default:

    printf("ERROR: Unsure how to generate asm with stmt type %d", stmt->type);
    exit(EXIT_FAILURE);
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

void generate_from_nodes(FILE *out, Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.type == EXPR)
      generate_expr(out, node.expr, env);
    else
      generate_stmt(out, node.stmt, env);
  }
}

void generate(FILE *out, Env *env, NodeList *program) {
  fprintf(out, "section .data\n");
  generate_vars(out, env);

  fprintf(out, "\nsection .text\n");
  generate_from_nodes(out, env, program);

  fclose(out);
}
