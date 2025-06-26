#include "../../include/lexer/lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *keywords[] = {"var", "func"};

int eat(char *str) {
  if (str == NULL)
    return -1;

  for (int i = 1; i < strlen(str); i++) {
    str[i - 1] = str[i];
  }
  str[strlen(str) - 1] = '\0';

  return 0;
}

int is_skippable(char c) {
  if (c == ' ' || c == '\n' || c == '\t')
    return EXIT_SUCCESS;

  else
    return EXIT_FAILURE;
}

TokenList *tokenize(char *path) {
  // TODO: Read from file path
  TokenList *tokens = create_token_list(10);

  char *mutable = malloc(strlen(path) + 1);
  strcpy(mutable, path);

  while (mutable[0] != '\0') {
    switch (mutable[0]) {
    case '(':
      add_to_token_list(tokens, create_token(PAREN_L, "("));
      eat(mutable);
      break;

    case ')':
      add_to_token_list(tokens, create_token(PAREN_R, ")"));
      eat(mutable);
      break;

    case '+':
      add_to_token_list(tokens, create_token(BIN_OP, "+"));
      eat(mutable);
      break;

    case '-':
      add_to_token_list(tokens, create_token(BIN_OP, "-"));
      eat(mutable);
      break;

    case '*':
      add_to_token_list(tokens, create_token(BIN_OP, "*"));
      eat(mutable);
      break;

    case '/':
      add_to_token_list(tokens, create_token(BIN_OP, "/"));
      eat(mutable);
      break;

    case '=':
      add_to_token_list(tokens, create_token(ASSIGNMENT, "="));
      eat(mutable);
      break;

    case ';':
      add_to_token_list(tokens, create_token(NEWLINE, "newline"));
      eat(mutable);
      break;

    // More than one char long tokens
    default: {
      char buffer[100] = "";
      int i = 0;

      // doubles & ints
      if (isnumber(mutable[0]) != 0) {
        int is_decimal = -1;
        while (isnumber(mutable[0]) != 0 || mutable[0] == '.') {
          if (mutable[0] == '.')
            is_decimal = 0;

          buffer[i] = mutable[0];
          i++;
          eat(mutable);
        }

        if (is_decimal == 0)
          add_to_token_list(tokens, create_token(DOUBLE, buffer));
        else
          add_to_token_list(tokens, create_token(INT, buffer));
      }

      //  keywords
      else if (is_skippable(mutable[0]) != 0) {
        while (mutable[0] != ' ' && mutable[0] != ';') {
          buffer[i] = mutable[0];
          i++;
          eat(mutable);
        }

        int found = -1;
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
          if (strcmp(keywords[i], buffer) == 0) {
            found = 0;
            break;
          }
        }

        if (found == 0)
          add_to_token_list(tokens, create_token(KEYWORD, buffer));
        else
          add_to_token_list(tokens, create_token(IDENTIFIER, buffer));
      } else
        eat(mutable);

      break;
    }
    }
  }

  return tokens;
}
