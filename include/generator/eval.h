#ifndef EVAL_H
#define EVAL_H

#include "../parser/ast.h"
#include "../parser/env.h"

VarValue eval(Bin *bin, Env *env);

#endif
