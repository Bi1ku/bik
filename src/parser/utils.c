#include "../../include/lexer/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token peek(TokenList *tokens) { return (tokens->tokens)[0]; }

Token eat(TokenList *tokens) {
  Token token = peek(tokens);

  for (int i = 1; i < tokens->size; i++) {
    tokens->tokens[i - 1] = tokens->tokens[i];
  }
  tokens->size--;

  return token;
}

Token expect(TokenList *tokens, TokenType type) {
  Token token = eat(tokens);

  if (token.type != type) {
    printf("ERROR: Expected token of type %d but got %d\n", type, token.type);
    exit(EXIT_FAILURE);
  }

  return token;
}

char *gen_id() {
  char *res = malloc(sizeof(char) * 16);

  for (int i = 0; i < 16; i++) {
    res[i] = 'A' + (random() % 26);
  }

  return res;
}
