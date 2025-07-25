#ifndef UTILS_H
#define UTILS_H

#include "../lexer/tokens.h"

Token peek(TokenList *tokens);
Token eat(TokenList *tokens);
Token expect(TokenList *tokens, TokenType type);
char *gen_id();

#endif
