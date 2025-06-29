#ifndef PARSER_H
#define PARSER_H

#include "../lexer/tokens.h"
#include "../parser/env.h"
#include "ast.h"

ProgramStmt *parse(TokenList *tokens, Env *env);

#endif
