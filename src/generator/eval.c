#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Expr *copy_expr(Expr *src) {
  Expr *expr = malloc(sizeof(Expr));

  *expr = *src;
  if (expr->type == STRING)

    expr->str = strdup(src->str);
  else if (expr->type == IDENTIFIER_EX)
    expr->identifier = strdup(src->identifier);

  return expr;
}

Expr *calculate(Expr *left, Expr *right, char op) {
  Expr *x = copy_expr(left);
  Expr *y = copy_expr(right);

  switch (op) {
  case '+': {
    char buffer[100];

    if (x->type == STRING) {
      if (y->type == INT) {
        sprintf(buffer, "%d", y->integer);
        x->str = strcat(x->str, buffer);
      }

      else if (y->type == FLOAT) {
        sprintf(buffer, "%f", y->floating);
        x->str = strcat(x->str, buffer);
      }

      else
        strcat(x->str, y->str);
    }

    else if (y->type == STRING) {
      if (x->type == INT) {
        sprintf(buffer, "%d", x->integer);
        x->str = strcat(buffer, y->str);
      }

      else if (x->type == FLOAT) {
        sprintf(buffer, "%f", x->floating);
        x->str = strcat(buffer, y->str);
      }

      x->type = STRING;
    }

    else {
      if (x->type == INT && y->type == FLOAT) {
        x->floating = x->integer + y->floating;
        x->type = FLOAT;
      } else if (x->type == FLOAT && y->type == INT) {
        x->floating = x->floating + y->integer;
        x->type = FLOAT;
      } else if (x->type == FLOAT && y->type == FLOAT) {
        x->floating = x->floating + y->floating;
        x->type = FLOAT;
      } else {
        x->integer = x->integer + y->integer;
        x->type = INT;
      }
    }

    free(y);
    return x;
  }

  case '-':
    if (x->type == INT && y->type == FLOAT) {
      x->floating = x->integer - y->floating;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == INT) {
      x->floating = x->floating - y->integer;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == FLOAT) {
      x->floating = x->floating - y->floating;
      x->type = FLOAT;
    } else if (x->type == INT && y->type == INT) {
      x->integer = x->integer - y->integer;
      x->type = INT;
    }

    else {
      printf("ERROR: '-' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    free(y);
    return x;

  case '*':
    if (x->type == INT && y->type == FLOAT) {
      x->floating = x->integer * y->floating;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == INT) {
      x->floating = x->floating * y->integer;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == FLOAT) {
      x->floating = x->floating * y->floating;
      x->type = FLOAT;
    } else if (x->type == INT && y->type == INT) {
      x->integer = x->integer * y->integer;
      x->type = INT;
    }

    else {
      printf("ERROR: '*' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    free(y);
    return x;

  case '/':
    if (x->type == INT && y->type == FLOAT) {
      x->floating = x->integer / y->floating;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == INT) {
      x->floating = x->floating / y->integer;
      x->type = FLOAT;
    } else if (x->type == FLOAT && y->type == FLOAT) {
      x->floating = x->floating / y->floating;
      x->type = FLOAT;
    } else if (x->type == INT && y->type == INT) {
      x->integer = x->integer / y->integer;
      x->type = INT;
    }

    else {
      printf("ERROR: '/' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    free(y);
    return x;

  default:
    printf("ERROR: Unexpected operand '%c'", op);
    exit(EXIT_FAILURE);
  }
}

Expr *get_val(Expr *expr, VarList *vars) {
  if (expr->type == INT || expr->type == FLOAT || expr->type == STRING)
    return expr;

  else if (expr->type == IDENTIFIER_EX)
    return get_var(vars, expr->identifier);

  else if (expr->type == BIN)
    return eval(expr->bin, vars);

  else {
    printf("ERROR: Trying to get invalid expression type!");
    exit(EXIT_FAILURE);
  }
}

Expr *eval(Bin *bin_expr, VarList *vars) {
  Expr *res;
  Expr *right = get_val(bin_expr->right, vars);

  res = get_val(bin_expr->left, vars);
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
