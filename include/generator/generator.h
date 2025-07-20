#ifndef GENERATOR_H
#define GENERATOR_H

#include "../parser/ast.h"
#include "../parser/env.h"

void generate(char *filename, Env *env, NodeList *program);

#endif
