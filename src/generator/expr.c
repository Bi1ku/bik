#include "../../include/generator/eval.h"
#include "../../include/generator/generator.h"
#include "../../include/parser/env.h"
#include <stdbool.h>
#include <stdlib.h>

void generate_expr(FILE *out, Expr *expr, Env *env, bool ret, char *var_name) {
  switch (expr->type) {
  case INT:
    if (ret) {
      if (var_name != NULL)
        fprintf(out, "\tli $v0, %d\n", expr->integer);
    } else {
      fprintf(out, "\tli $t%d, %d\n", t_reg, expr->integer);
      t_reg++;
    }

    break;

  case FLOAT:
    if (var_name) {
      fprintf(out, "\tl.s $f%d, %s\n", f_reg, var_name);
      f_reg++;
    }
    break;

  case IDENTIFIER_EX: {
    Expr *var = get_var(env->items, expr->identifier);
    generate_expr(out, var, env, ret, expr->identifier);
    break;
  }

  case BIN: {
    Expr *res = eval(expr->bin, env->items);
    generate_expr(out, res, env, ret, NULL);
    break;
  }

  case STRING:
    if (var_name) {
      ret ? fprintf(out, "\tla $v0, %s\n", var_name)
          : fprintf(out, "\tla $t%d, %s\n", t_reg, var_name);
    } else {
      printf("ERROR: Haven't done string asm logic yet");
      exit(EXIT_FAILURE);
    }

  default:
    printf("ERROR: Unsure how to generate asm with expr type %d", expr->type);
    exit(EXIT_FAILURE);
  }
}
