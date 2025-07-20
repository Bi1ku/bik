#ifndef GENERATOR_H
#define GENERATOR_H

#include "../parser/ast.h"
#include "../parser/env.h"
#include <stdio.h>

void generate(FILE *out, Env *env, NodeList *program);

#endif
