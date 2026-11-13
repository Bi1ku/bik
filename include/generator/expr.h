#ifndef GENERATOR_EXPR_H
#define GENERATOR_EXPR_H

#include "../../include/parser/env.h"
#include <stdbool.h>
#include <stdio.h>

void generate_expr(FILE *out, Expr *expr, Env *env, bool ret, char *var_name);
void generate_int_expr(FILE *out, Expr *expr, Env *env, int depth);

#endif
