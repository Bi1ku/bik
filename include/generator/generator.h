#ifndef GENERATOR_H
#define GENERATOR_H

#include "../parser/ast.h"
#include "../parser/env.h"
#include <stdio.h>

extern int t_reg;
extern int f_reg;
extern int a_reg;

void generate_from_nodes(FILE *out, Env *env, NodeList *program);
void generate(FILE *out, Env *env, NodeList *program);

#endif
