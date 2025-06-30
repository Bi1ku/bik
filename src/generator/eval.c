#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <string.h>

double eval(BinaryExpr *bin_expr, Env *env);

double get_val(Expr *expr, Env *env) {
  if (expr->type == BIN_EXPR)
    return eval(expr->bin_expr, env);

  else if (expr->type == INT_EXPR)
    return expr->int_expr->value;

  else if (expr->type == IDENTIFIER_EXPR) {
    Var var = get_var(env, expr->identifier_expr->symbol);

    if (var.type == DOUBLE)
      return var.double_val;

    return var.int_val;
  }

  return expr->double_expr->value;
}

double eval(BinaryExpr *bin_expr, Env *env) {
  double res = 0;
  double right = get_val(bin_expr->right, env);

  res = get_val(bin_expr->left, env);
  if (strcmp(bin_expr->op, "+") == 0)
    res += right;

  else if (strcmp(bin_expr->op, "-") == 0)
    res -= right;

  else if (strcmp(bin_expr->op, "*") == 0)
    res *= right;

  else if (strcmp(bin_expr->op, "/") == 0)
    res /= right;

  // TODO: modulo

  return res;
}

Var normalize(double num) {
  Var val;
  if (num == (int)num) {
    val.type = INT;
    val.int_val = num;
  } else {
    val.type = DOUBLE;
    val.double_val = num;
  }

  return val;
}
