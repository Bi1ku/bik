#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Expr *calculate(Expr *left, Expr *right, char op) {
  Expr *res = malloc(sizeof(Expr));

  switch (op) {
  case '+': {
    char *buffer = malloc(100 * sizeof(char));

    res->type = STRING;
    res->str = malloc(sizeof(Str));

    if (left->type == STRING) {
      if (right->type == INT) {
        sprintf(buffer, "%d", right->integer);
        res->str->val = strdup(left->str->val);
        strcat(res->str->val, buffer);
      } else if (right->type == FLOAT) {
        sprintf(buffer, "%f", right->floating->val);
        res->str->val = strdup(left->str->val);
        strcat(res->str->val, buffer);
      } else {
        res->str->val = strdup(left->str->val);
        strcat(res->str->val, right->str->val);
      }
    }

    else if (right->type == STRING) {
      if (left->type == INT) {
        sprintf(buffer, "%d", left->integer);
        res->str->val = strcat(buffer, right->str->val);
      } else if (left->type == FLOAT) {
        sprintf(buffer, "%f", left->floating->val);
        res->str->val = strcat(buffer, right->str->val);
      }
    }

    else {
      res->floating = malloc(sizeof(Float));
      res->type = FLOAT;

      if (left->type == INT && right->type == FLOAT)
        res->floating->val = left->integer + right->floating->val;
      else if (left->type == FLOAT && right->type == INT)
        res->floating->val = left->floating->val + right->integer;
      else if (left->type == FLOAT && right->type == FLOAT)
        res->floating->val = left->floating->val + right->floating->val;
      else {
        free(res->floating);
        res->type = INT;
        res->integer = left->integer + right->integer;
      }
    }

    return res;
  }

  case '-':
    res->floating = malloc(sizeof(Float));
    res->type = FLOAT;

    if (left->type == INT && right->type == FLOAT)
      res->floating->val = left->integer - right->floating->val;
    else if (left->type == FLOAT && right->type == INT)
      res->floating->val = left->floating->val - right->integer;
    else if (left->type == FLOAT && right->type == FLOAT)
      res->floating->val = left->floating->val - right->floating->val;
    else if (left->type == INT && right->type == INT) {
      free(res->floating);
      res->type = INT;
      res->integer = left->integer - right->integer;
    }

    else {
      printf("ERROR: '-' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return res;

  case '*':
    res->floating = malloc(sizeof(Float));
    res->type = FLOAT;

    if (left->type == INT && right->type == FLOAT)
      res->floating->val = left->integer * right->floating->val;
    else if (left->type == FLOAT && right->type == INT)
      res->floating->val = left->floating->val * right->integer;
    else if (left->type == FLOAT && right->type == FLOAT)
      res->floating->val = left->floating->val * right->floating->val;
    else if (left->type == INT && right->type == INT) {
      free(res->floating);
      res->type = INT;
      res->integer = left->integer * right->integer;
    }

    else {
      printf("ERROR: '*' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return res;

  case '/':
    res->floating = malloc(sizeof(Float));
    res->type = FLOAT;

    if (left->type == INT && right->type == FLOAT)
      res->floating->val = left->integer / right->floating->val;
    else if (left->type == FLOAT && right->type == INT)
      res->floating->val = left->floating->val / right->integer;
    else if (left->type == FLOAT && right->type == FLOAT)
      res->floating->val = left->floating->val / right->floating->val;
    else if (left->type == INT && right->type == INT) {
      free(res->floating);
      res->type = INT;
      res->integer = left->integer / right->integer;
    }

    else {
      printf("ERROR: '/' operand not permitted on strings");
      exit(EXIT_FAILURE);
    }

    return res;

  default:
    printf("ERROR: Unexpected operand '%c'", op);
    exit(EXIT_FAILURE);
  }
}

Expr *get_val(Expr *expr, VarList *vars) {
  switch (expr->type) {
  case IDENTIFIER_EX:
    return get_var(vars, expr->identifier);

  case BIN:
    return eval(expr->bin, vars);

  default:
    if (expr->type == INT || expr->type == FLOAT || expr->type == STRING)
      return expr;

    printf("ERROR: Trying to get invalid expression type!");
    exit(EXIT_FAILURE);
  }
}

Expr *eval(Bin *bin_expr, VarList *vars) {
  Expr *res = get_val(bin_expr->left, vars);
  Expr *right = get_val(bin_expr->right, vars);
  return calculate(res, right, bin_expr->op[0]);
}
