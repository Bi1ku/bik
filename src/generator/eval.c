#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include <stdio.h>
#include <string.h>

double eval(BinaryExpr *bin_expr);

double get_value(Expr *expr) {
  if (expr->type == BIN_EXPR)
    return eval(expr->bin_expr);
  else if (expr->type == INT_EXPR)
    return expr->int_expr->value;
  else // DOUBLE_EXPR
    return expr->double_expr->value;
}

double eval(BinaryExpr *bin_expr) {
  double res = 0;
  double right = get_value(bin_expr->right);

  res = get_value(bin_expr->left);
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

Value normalize(double num) {
  Value val;
  if (num == (int)num) {
    val.type = V_INT;
    val.int_value = num;
  } else {
    val.type = V_DOUBLE;
    val.double_value = num;
  }

  return val;
}
