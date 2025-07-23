#ifndef PARSER_H
#define PARSER_H

#include "../lexer/tokens.h"
#include "../parser/env.h"
#include "ast.h"

NodeList *parse(TokenList *tokens, Env *env);
NodeList *parse_line(NodeList *nodes, TokenList *tokens, Env *env);

#endif
