#include "../../include/generator/eval.h"
#include "../../include/generator/generator.h"
#include "../../include/parser/env.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parameter_index(const char *name) {
  if (current_func == NULL)
    return -1;

  for (int i = 0; i < current_func->params->size; i++) {
    Param *param = current_func->params->nodes[i].stmt->param;
    if (strcmp(param->name, name) == 0)
      return i;
  }
  return -1;
}

void generate_int_expr(FILE *out, Expr *expr, Env *env, int depth) {
  if (depth > 8) {
    fprintf(stderr, "ERROR: Expression is too deeply nested for register allocation.\n");
    exit(EXIT_FAILURE);
  }

  switch (expr->type) {
  case INT:
    fprintf(out, "\tli $t%d, %d\n", depth, expr->integer);
    return;

  case IDENTIFIER_EX: {
    int index = parameter_index(expr->identifier);
    if (index >= 0) {
      fprintf(out, "\tmove $t%d, $a%d\n", depth, index);
      return;
    }

    Expr *value = get_var(env->items, expr->identifier);
    if (value == NULL) {
      fprintf(stderr, "ERROR: Unknown variable '%s'.\n", expr->identifier);
      exit(EXIT_FAILURE);
    }
    if (value->type == INT) {
      fprintf(out, "\tlw $t%d, %s\n", depth, expr->identifier);
      return;
    }
    fprintf(stderr, "ERROR: Variable '%s' is not an integer.\n", expr->identifier);
    exit(EXIT_FAILURE);
  }

  case BIN: {
    generate_int_expr(out, expr->bin->left, env, depth);
    generate_int_expr(out, expr->bin->right, env, depth + 1);
    if (strcmp(expr->bin->op, "+") == 0)
      fprintf(out, "\taddu $t%d, $t%d, $t%d\n", depth, depth, depth + 1);
    else if (strcmp(expr->bin->op, "-") == 0)
      fprintf(out, "\tsubu $t%d, $t%d, $t%d\n", depth, depth, depth + 1);
    else if (strcmp(expr->bin->op, "*") == 0) {
      fprintf(out, "\tmult $t%d, $t%d\n", depth, depth + 1);
      fprintf(out, "\tmflo $t%d\n", depth);
    } else if (strcmp(expr->bin->op, "/") == 0) {
      fprintf(out, "\tdiv $t%d, $t%d\n", depth, depth + 1);
      fprintf(out, "\tmflo $t%d\n", depth);
    } else if (strcmp(expr->bin->op, "%") == 0) {
      fprintf(out, "\tdiv $t%d, $t%d\n", depth, depth + 1);
      fprintf(out, "\tmfhi $t%d\n", depth);
    } else {
      fprintf(stderr, "ERROR: Unsupported integer operator '%s'.\n", expr->bin->op);
      exit(EXIT_FAILURE);
    }
    return;
  }

  default:
    fprintf(stderr, "ERROR: Expected an integer expression, got type %d.\n", expr->type);
    exit(EXIT_FAILURE);
  }
}

void generate_expr(FILE *out, Expr *expr, Env *env, bool ret, char *var_name) {
  (void)var_name;
  if (expr->type == FLOAT) {
    if (ret)
      fprintf(out, "\tli.s $f0, %f\n", expr->floating->val);
    else
      fprintf(out, "\tli.s $f%d, %f\n", f_reg++, expr->floating->val);
    return;
  }

  generate_int_expr(out, expr, env, 0);
  if (ret)
    fprintf(out, "\tmove $v0, $t0\n");
}
