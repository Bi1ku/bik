#include "../../include/lexer/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token create_token(TokenType type, char *value) {
  Token token;
  token.type = type;
  token.value = strdup(value); // don't reference the original

  return token;
}

TokenList *create_token_list(int init_size) {
  TokenList *tokens = malloc(sizeof(TokenList));
  tokens->capacity = init_size;
  tokens->size = 0;
  tokens->tokens = malloc(init_size * sizeof(Token));

  return tokens;
}

void resize(TokenList *tokens) {
  tokens->capacity *= 2;
  tokens->tokens = realloc(tokens->tokens, tokens->capacity * sizeof(Token));
}

void add_to_token_list(TokenList *tokens, Token token) {
  if (tokens->size >= tokens->capacity)
    resize(tokens);

  tokens->tokens[tokens->size] = token;
  tokens->size++;
}

char *get_string_token_type(int index) {
  switch (index) {
  case 0:
    return "BIN_OP";
  case 1:
    return "PAREN_L";
  case 2:
    return "PAREN_R";
  case 3:
    return "NUM";
  case 4:
    return "KEYWORD";
  case 5:
    return "IDENTIFIER";
  case 6:
    return "NEWLINE";
  case 7:
    return "ASSIGNMENT";
  case 8:
    return "END";
  case 9:
    return "STRING";
  default:
    return "UNKNOWN";
  }
}

void print_tokens(TokenList *tokens) {
  printf("{\n");
  for (int i = 0; i < tokens->size - 1; i++) {
    printf("\t{ %s: %s },\n", get_string_token_type(tokens->tokens[i].type),
           tokens->tokens[i].value);
  }
  printf("\t{ %s: %s }\n}\n",
         get_string_token_type(tokens->tokens[tokens->size - 1].type),
         tokens->tokens[tokens->size - 1].value);
}
