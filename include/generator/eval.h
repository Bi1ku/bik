#ifndef EVAL_H
#define EVAL_H

#include "../parser/ast.h"
#include "../parser/env.h"

Var eval(BinaryExpr *bin_expr, Env *env);

#endif
