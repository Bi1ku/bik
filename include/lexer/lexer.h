#ifndef LEXER_H
#define LEXER_H
#include "tokens.h"

TokenList *tokenize(char *path);

char *read(char *path);

#endif
