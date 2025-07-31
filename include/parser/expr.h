#ifndef EXPR_H
#define EXPR_H

#include "../lexer/tokens.h"
#include "ast.h"
#include "env.h"

Node *parse_additive(TokenList *tokens, char *mem, VarList *vars);
Node *parse_values(TokenList *tokens, char *mem, VarList *vars);
Node *parse_multiplicative(TokenList *tokens, char *mem, VarList *vars);
Node *parse_additive(TokenList *tokens, char *mem, VarList *vars);

#endif
