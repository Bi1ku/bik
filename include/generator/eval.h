#ifndef EVAL_H
#define EVAL_H

#include "../parser/ast.h"
#include "../parser/env.h"

typedef enum { V_INT, V_DOUBLE } ValueType;

typedef struct {
  ValueType type;
  union {
    int int_value;
    double double_value;
  };
} Value;

double eval(BinaryExpr *bin_expr, Env *env);

Value normalize(double num);

#endif
