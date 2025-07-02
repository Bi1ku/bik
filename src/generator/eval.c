#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Var calculate(Var x, Var y, char op) {
  switch (op) {
  case '+': {
    char buffer[100];

    if (x.type == STRING) {
      if (y.type == INT) {
        sprintf(buffer, "%d", y.int_val);
        x.str_val = strcat(x.str_val, buffer);
      }

      else if (y.type == DOUBLE) {
        sprintf(buffer, "%f", y.double_val);
        x.str_val = strcat(x.str_val, buffer);
      }

      else
        strcat(x.str_val, y.str_val);
    }

    else if (y.type == STRING) {
      if (x.type == INT) {
        sprintf(buffer, "%d", x.int_val);
        x.str_val = strcat(buffer, y.str_val);
      }

      else if (x.type == DOUBLE) {
        sprintf(buffer, "%f", x.double_val);
        x.str_val = strcat(buffer, y.str_val);
      }

      x.type = STRING;
    }

    else {
      if (x.type == INT && y.type == DOUBLE) {
        x.double_val = x.int_val + y.double_val;
        x.type = DOUBLE;
      } else if (x.type == DOUBLE && y.type == INT) {
        x.double_val = x.double_val + y.int_val;
        x.type = DOUBLE;
      } else if (x.type == DOUBLE && y.type == DOUBLE) {
        x.double_val = x.double_val + y.double_val;
        x.type = DOUBLE;
      } else {
        x.int_val = x.int_val + y.int_val;
        x.type = INT;
      }
    }

    return x;
  }

  case '-':
    if (x.type == INT && y.type == DOUBLE) {
      x.double_val = x.int_val - y.double_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == INT) {
      x.double_val = x.double_val - y.int_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == DOUBLE) {
      x.double_val = x.double_val - y.double_val;
      x.type = DOUBLE;
    } else if (x.type == INT && y.type == INT) {
      x.int_val = x.int_val - y.int_val;
      x.type = INT;
    }

    else {
      printf("ERROR: '-' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return x;

  case '*':
    if (x.type == INT && y.type == DOUBLE) {
      x.double_val = x.int_val * y.double_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == INT) {
      x.double_val = x.double_val * y.int_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == DOUBLE) {
      x.double_val = x.double_val * y.double_val;
      x.type = DOUBLE;
    } else if (x.type == INT && y.type == INT) {
      x.int_val = x.int_val * y.int_val;
      x.type = INT;
    }

    else {
      printf("ERROR: '*' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return x;

  case '/':
    if (x.type == INT && y.type == DOUBLE) {
      x.double_val = x.int_val / y.double_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == INT) {
      x.double_val = x.double_val / y.int_val;
      x.type = DOUBLE;
    } else if (x.type == DOUBLE && y.type == DOUBLE) {
      x.double_val = x.double_val / y.double_val;
      x.type = DOUBLE;
    } else if (x.type == INT && y.type == INT) {
      x.int_val = x.int_val / y.int_val;
      x.type = INT;
    }

    else {
      printf("ERROR: '/' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return x;

  default:
    printf("ERROR: Unexpected operand '%c'", op);
    exit(EXIT_FAILURE);
  }
}

Var get_val(Expr *expr, Env *env) {
  if (expr->type == BIN_EXPR)
    return eval(expr->bin_expr, env);

  else if (expr->type == INT_EXPR) {
    return create_int_var("temp", expr->int_expr->value).value;
  }

  else if (expr->type == IDENTIFIER_EXPR) {
    return *get_var(env->items, expr->identifier_expr->symbol);
  }

  else if (expr->type == STRING_EXPR) {
    return create_str_var("temp", expr->string_expr->value).value;
  }

  return create_double_var("temp", expr->double_expr->value).value;
}

Var eval(BinaryExpr *bin_expr, Env *env) {
  Var res;
  Var right = get_val(bin_expr->right, env);

  res = get_val(bin_expr->left, env);
  if (strcmp(bin_expr->op, "+") == 0)
    res = calculate(res, right, '+');

  else if (strcmp(bin_expr->op, "-") == 0)
    res = calculate(res, right, '-');

  else if (strcmp(bin_expr->op, "*") == 0)
    res = calculate(res, right, '*');

  else if (strcmp(bin_expr->op, "/") == 0)
    res = calculate(res, right, '/');

  // TODO: modulo

  return res;
}
