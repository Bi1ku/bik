#ifndef STMT_H
#define STMT_H

#include "../../include/parser/env.h"
#include <stdio.h>

void generate_log(FILE *out, Expr *expr, Env *env, char *var_name);
void generate_stmt(FILE *out, Stmt *stmt, Env *env);
void generate_funcs(FILE *out, Env *env, NodeList *program);

#endif
