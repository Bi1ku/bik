#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
  BIN_OP,
  PAREN_R,
  PAREN_L,
  NUM,
  KEYWORD,
  IDENTIFIER,
  NEWLINE,
  ASSIGNMENT,
  END,
  STR,
} TokenType;

typedef struct {
  TokenType type;
  char *value;
} Token;

typedef struct {
  int capacity;
  int size;
  Token *tokens;
} TokenList;

Token create_token(TokenType type, char value[]);

TokenList *create_token_list(int init_size);

void add_to_token_list(TokenList *tokens, Token token);

void print_tokens(TokenList *tokens);

#endif
