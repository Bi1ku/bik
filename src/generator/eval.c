#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VarValue calculate(VarValue x, VarValue y, char op) {
  switch (op) {
  case '+': {
    char buffer[100];

    if (x.type == STRING) {
      if (y.type == INT) {
        sprintf(buffer, "%d", y.int_val);
        x.str_val = strcat(x.str_val, buffer);
      }

      else if (y.type == FLOAT) {
        sprintf(buffer, "%f", y.float_val);
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

      else if (x.type == FLOAT) {
        sprintf(buffer, "%f", x.float_val);
        x.str_val = strcat(buffer, y.str_val);
      }

      x.type = STRING;
    }

    else {
      if (x.type == INT && y.type == FLOAT) {
        x.float_val = x.int_val + y.float_val;
        x.type = FLOAT;
      } else if (x.type == FLOAT && y.type == INT) {
        x.float_val = x.float_val + y.int_val;
        x.type = FLOAT;
      } else if (x.type == FLOAT && y.type == FLOAT) {
        x.float_val = x.float_val + y.float_val;
        x.type = FLOAT;
      } else {
        x.int_val = x.int_val + y.int_val;
        x.type = INT;
      }
    }

    return x;
  }

  case '-':
    if (x.type == INT && y.type == FLOAT) {
      x.float_val = x.int_val - y.float_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == INT) {
      x.float_val = x.float_val - y.int_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == FLOAT) {
      x.float_val = x.float_val - y.float_val;
      x.type = FLOAT;
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
    if (x.type == INT && y.type == FLOAT) {
      x.float_val = x.int_val * y.float_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == INT) {
      x.float_val = x.float_val * y.int_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == FLOAT) {
      x.float_val = x.float_val * y.float_val;
      x.type = FLOAT;
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
    if (x.type == INT && y.type == FLOAT) {
      x.float_val = x.int_val / y.float_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == INT) {
      x.float_val = x.float_val / y.int_val;
      x.type = FLOAT;
    } else if (x.type == FLOAT && y.type == FLOAT) {
      x.float_val = x.float_val / y.float_val;
      x.type = FLOAT;
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

VarValue get_val(Expr *expr, Env *env) {
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

  return create_float_var("temp", expr->float_expr->value).value;
}

VarValue eval(BinaryExpr *bin_expr, Env *env) {
  VarValue res;
  VarValue right = get_val(bin_expr->right, env);

  res = get_val(bin_expr->left, env);
  if (strcmp(bin_expr->op, "+") == 0)
    res = calculate(res, right, '+');

  else if (strcmp(bin_expr->op, "-") == 0)
    res = calculate(res, right, '-');

  else if (strcmp(bin_expr->op, "*") == 0)
    res = calculate(res, right, '*');

  else if (strcmp(bin_expr->op, "/") == 0)
    res = calculate(res, right, '/');

  return res;
}
