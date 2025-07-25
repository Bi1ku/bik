#ifndef STMT_H
#define STMT_H

#include "../lexer/tokens.h"
#include "env.h"

NodeList *parse_func_args(TokenList *tokens);
Node *parse_stmt(NodeList *nodes, TokenList *tokens, Env *env);

#endif
