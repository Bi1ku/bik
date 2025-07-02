#ifndef EVAL_H
#define EVAL_H

#include "../parser/ast.h"
#include "../parser/env.h"

double eval(BinaryExpr *bin_expr, Env *env);

Var normalize(double num);

#endif
