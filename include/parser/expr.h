#ifndef EXPR_H
#define EXPR_H

#include "../lexer/tokens.h"
#include "ast.h"

Node *parse_additive(TokenList *tokens);
Node *parse_values(TokenList *tokens);
Node *parse_multiplicative(TokenList *tokens);
Node *parse_additive(TokenList *tokens);

#endif
