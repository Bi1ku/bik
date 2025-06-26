#ifndef PARSER_H
#define PARSER_H

#include "../lexer/tokens.h"
#include "ast.h"

ProgramStmt *parse(TokenList *tokens);

#endif
