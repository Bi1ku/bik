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
    if (left->type == STRING || right->type == STRING) {
      char left_number[64], right_number[64];
      if (left->type == INT)
        snprintf(left_number, sizeof(left_number), "%d", left->integer);
      else if (left->type == FLOAT)
        snprintf(left_number, sizeof(left_number), "%f", left->floating->val);
      if (right->type == INT)
        snprintf(right_number, sizeof(right_number), "%d", right->integer);
      else if (right->type == FLOAT)
        snprintf(right_number, sizeof(right_number), "%f", right->floating->val);
      char *a = left->type == STRING ? left->str->val : left_number;
      char *b = right->type == STRING ? right->str->val : right_number;
      char *value = malloc(strlen(a) + strlen(b) + 1);
      strcpy(value, a);
      strcat(value, b);
      free(res);
      return create_string_expr(value, NULL);
    }

    else {
      res->floating = calloc(1, sizeof(Float));
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
    res->floating = calloc(1, sizeof(Float));
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
    res->floating = calloc(1, sizeof(Float));
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

  case '%':
    if (left->type != INT || right->type != INT) {
      fprintf(stderr, "ERROR: Remainder requires integers.\n");
      exit(EXIT_FAILURE);
    }
    if (right->integer == 0) {
      fprintf(stderr, "ERROR: Division by zero.\n");
      exit(EXIT_FAILURE);
    }
    res->type = INT;
    res->integer = left->integer % right->integer;
    return res;

  case '/':
    if ((right->type == INT && right->integer == 0) ||
        (right->type == FLOAT && right->floating->val == 0)) {
      fprintf(stderr, "ERROR: Division by zero.\n");
      exit(EXIT_FAILURE);
    }
    res->floating = calloc(1, sizeof(Float));
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
  case IDENTIFIER_EX: {
    Expr *value = get_var(vars, expr->identifier);
    if (value == NULL) {
      fprintf(stderr, "ERROR: Unknown variable '%s'.\n", expr->identifier);
      exit(EXIT_FAILURE);
    }
    return value;
  }

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
