#include "../../include/lexer/lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *keywords[] = {"func", "ret", "log", "str", "int", "deci", "if"};

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
      if (mutable[1] == '=') {
        add_to_token_list(tokens, create_token(COMPARE, "=="));
        shift(mutable);
        shift(mutable);
      } else {
        add_to_token_list(tokens, create_token(ASSIGNMENT, "="));
        shift(mutable);
      }
      break;

    case '!':
      if (mutable[1] == '=') {
        add_to_token_list(tokens, create_token(COMPARE, "!="));
        shift(mutable);
        shift(mutable);
      } else {
        shift(mutable);
      }
      break;

    case '<':
    case '>': {
      char op[3] = {mutable[0], '\0', '\0'};
      if (mutable[1] == '=') {
        op[1] = '=';
        shift(mutable);
      }
      add_to_token_list(tokens, create_token(COMPARE, op));
      shift(mutable);
      break;
    }

    case '{':
      add_to_token_list(tokens, create_token(BLOCK_START, "{"));
      shift(mutable);
      break;

    case '}':
      add_to_token_list(tokens, create_token(BLOCK_END, "}"));
      shift(mutable);
      break;

    case ',':
      add_to_token_list(tokens, create_token(COMMA, ","));
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
      if (isdigit((unsigned char)mutable[0]) != 0) {
        while (isdigit((unsigned char)mutable[0]) != 0 || mutable[0] == '.') {
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
        bool params = false;

        while (quotes || (mutable[0] != ' ' && mutable[0] != ';')) {
          if (mutable[0] == '"') {
            quotes = !quotes;
            is_str = true;
          }

          else if (!quotes && (mutable[0] == '(' || mutable[0] == ')' ||
                               mutable[0] == ',' || mutable[0] == '='))
            break;

          else {
            buffer[i] = mutable[0];
            i++;
          }

          shift(mutable);
        }

        if (!params) {
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

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    printf("ERROR: Cannot seek in file %s\n", path);
    exit(EXIT_FAILURE);
  }

  long file_size = ftell(file);
  if (file_size < 0 || fseek(file, 0, SEEK_SET) != 0) {
    fclose(file);
    printf("ERROR: Cannot read file %s\n", path);
    exit(EXIT_FAILURE);
  }

  char *buffer = malloc((size_t)file_size + 1);
  if (buffer == NULL) {
    fclose(file);
    printf("ERROR: Could not allocate memory for file %s\n", path);
    exit(EXIT_FAILURE);
  }

  size_t bytes_read = fread(buffer, 1, (size_t)file_size, file);
  if (bytes_read != (size_t)file_size) {
    free(buffer);
    fclose(file);
    printf("ERROR: Could not read file %s\n", path);
    exit(EXIT_FAILURE);
  }

  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}
