#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Expr *calculate(Expr *x, Expr *y, char op) {
  switch (op) {
  case '+': {
    char buffer[100];

    if (x->type == STRING_EX) {
      if (y->type == INT_EX) {
        sprintf(buffer, "%d", y->integer);
        x->str = strcat(x->str, buffer);
      }

      else if (y->type == FLOAT_EX) {
        sprintf(buffer, "%f", y->floating);
        x->str = strcat(x->str, buffer);
      }

      else
        strcat(x->str, y->str);
    }

    else if (y->type == STRING_EX) {
      if (x->type == INT_EX) {
        sprintf(buffer, "%d", x->integer);
        x->str = strcat(buffer, y->str);
      }

      else if (x->type == FLOAT_EX) {
        sprintf(buffer, "%f", x->floating);
        x->str = strcat(buffer, y->str);
      }

      x->type = STRING_EX;
    }

    else {
      if (x->type == INT_EX && y->type == FLOAT_EX) {
        x->floating = x->integer + y->floating;
        x->type = FLOAT_EX;
      } else if (x->type == FLOAT_EX && y->type == INT_EX) {
        x->floating = x->floating + y->integer;
        x->type = FLOAT_EX;
      } else if (x->type == FLOAT_EX && y->type == FLOAT_EX) {
        x->floating = x->floating + y->floating;
        x->type = FLOAT_EX;
      } else {
        x->integer = x->integer + y->integer;
        x->type = INT_EX;
      }
    }

    return x;
  }

  case '-':
    if (x->type == INT_EX && y->type == FLOAT_EX) {
      x->floating = x->integer - y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == INT_EX) {
      x->floating = x->floating - y->integer;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == FLOAT_EX) {
      x->floating = x->floating - y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == INT_EX && y->type == INT_EX) {
      x->integer = x->integer - y->integer;
      x->type = INT_EX;
    }

    else {
      printf("ERROR: '-' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return x;

  case '*':
    if (x->type == INT_EX && y->type == FLOAT_EX) {
      x->floating = x->integer * y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == INT_EX) {
      x->floating = x->floating * y->integer;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == FLOAT_EX) {
      x->floating = x->floating * y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == INT_EX && y->type == INT_EX) {
      x->integer = x->integer * y->integer;
      x->type = INT_EX;
    }

    else {
      printf("ERROR: '*' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return x;

  case '/':
    if (x->type == INT_EX && y->type == FLOAT_EX) {
      x->floating = x->integer / y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == INT_EX) {
      x->floating = x->floating / y->integer;
      x->type = FLOAT_EX;
    } else if (x->type == FLOAT_EX && y->type == FLOAT_EX) {
      x->floating = x->floating / y->floating;
      x->type = FLOAT_EX;
    } else if (x->type == INT_EX && y->type == INT_EX) {
      x->integer = x->integer / y->integer;
      x->type = INT_EX;
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

Expr *get_val(Expr *expr, Env *env) {
  if (expr->type == INT_EX || expr->type == FLOAT_EX || expr->type == STRING_EX)
    return expr;

  else if (expr->type == IDENTIFIER_EX)
    return get_var(env->items, expr->identifier);

  else if (expr->type == BIN)
    return eval(expr->bin, env);

  else {

    printf("ERROR: Trying to get invalid expression type!");
    exit(EXIT_FAILURE);
  }
}

Expr *eval(Bin *bin_expr, Env *env) {
  Expr *res;
  Expr *right = get_val(bin_expr->right, env);

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
