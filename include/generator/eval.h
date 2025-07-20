#ifndef EVAL_H
#define EVAL_H

#include "../parser/ast.h"
#include "../parser/env.h"

Expr *eval(Bin *bin_expr, VarList *vars);

#endif
