#include "../../include/lexer/lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *keywords[] = {"func", "ret", "log"};

int shift(char *str) {
  if (str == NULL) {
    printf("ERROR: Cannot shift NULL string\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < strlen(str); i++) {
    str[i - 1] = str[i];
  }
  str[strlen(str) - 1] = '\0';

  return EXIT_SUCCESS;
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
      shift(mutable);
      break;

    case ')':
      add_to_token_list(tokens, create_token(PAREN_R, ")"));
      shift(mutable);
      break;

    case '+':
      add_to_token_list(tokens, create_token(BIN_OP, "+"));
      shift(mutable);
      break;

    case '-':
      add_to_token_list(tokens, create_token(BIN_OP, "-"));
      shift(mutable);
      break;

    case '*':
      add_to_token_list(tokens, create_token(BIN_OP, "*"));
      shift(mutable);
      break;

    case '/':
      if (mutable[1] == '/') {
        while (mutable[0] != '\n' && mutable[0] != '\0') {
          shift(mutable);
        }
        continue;
      }
      add_to_token_list(tokens, create_token(BIN_OP, "/"));
      shift(mutable);
      break;

    case '=':
      add_to_token_list(tokens, create_token(ASSIGNMENT, "="));
      shift(mutable);
      break;

    case '{':
      add_to_token_list(tokens, create_token(BLOCK_START, "{"));
      shift(mutable);
      break;

    case '}':
      add_to_token_list(tokens, create_token(BLOCK_END, "}"));
      shift(mutable);
      break;

    case ';':
      add_to_token_list(tokens, create_token(NEWLINE, "newline"));
      shift(mutable);
      break;

    // More than one char long tokens
    default: {
      char buffer[100] = "";
      int i = 0;

      // floats & ints
      if (isnumber(mutable[0]) != 0) {
        while (isnumber(mutable[0]) != 0 || mutable[0] == '.') {
          buffer[i] = mutable[0];
          i++;
          shift(mutable);
        }

        add_to_token_list(tokens, create_token(NUM, buffer));
      }

      //  keywords
      else if (is_skippable(mutable[0]) != 0) {
        bool quotes = false;
        bool is_str = false;
        bool comma = false;

        while ((mutable[0] != ' ' || quotes) && mutable[0] != ';') {
          if (mutable[0] == '"') {
            quotes = !quotes;
            is_str = true;
          }

          // for params
          else if ((mutable[0] == ',' && !quotes) || mutable[0] == ')') {
            comma = true;
            add_to_token_list(tokens, create_token(IDENTIFIER, buffer));
            if (mutable[0] == ',')
              add_to_token_list(tokens, create_token(COMMA, ","));
            else
              add_to_token_list(tokens, create_token(PAREN_R, ")"));
            shift(mutable);
            break;
          }

          else {
            buffer[i] = mutable[0];
            i++;
          }

          shift(mutable);
        }

        if (!comma) {
          if (quotes) {
            printf("ERROR: Quotes are wrong");
            exit(EXIT_FAILURE);
          }

          if (is_str) {
            add_to_token_list(tokens, create_token(STR, buffer));
            break;
          }

          bool found = false;
          for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
            if (strcmp(keywords[i], buffer) == 0) {
              found = true;
              break;
            }
          }

          if (found)
            add_to_token_list(tokens, create_token(KEYWORD, buffer));
          else
            add_to_token_list(tokens, create_token(IDENTIFIER, buffer));
        }
      } else
        shift(mutable);

      break;
    }
    }
  }

  add_to_token_list(tokens, create_token(END, "END"));
  return tokens;
}

char *read(char *path) {
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    printf("ERROR: Cannot open file %s\n", path);
    exit(EXIT_FAILURE);
  }

  char *buffer = malloc(sizeof(char) * 100); // todo: realloc later?
  char line[100];
  while (fgets(line, sizeof(line), file) != NULL) {
    strcat(buffer, line);
  }

  return buffer;
}
