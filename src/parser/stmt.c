#include "../../include/parser/expr.h"
#include "../../include/parser/parser.h"
#include "../../include/parser/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NodeList *parse_func_params(TokenList *tokens) {
  NodeList *nodes = create_node_list(5);

  while (peek(tokens).type != PAREN_R) {
    if (peek(tokens).type == KEYWORD) {
      char *type = eat(tokens).value;

      if (strcmp(type, "int") == 0)
        add_node_to_node_list(
            nodes, create_stmt_node(create_param_stmt(eat(tokens).value, INT)));
      else if (strcmp(type, "deci") == 0)
        add_node_to_node_list(nodes, create_stmt_node(create_param_stmt(
                                         eat(tokens).value, FLOAT)));
      else if (strcmp(type, "str") == 0)
        add_node_to_node_list(nodes, create_stmt_node(create_param_stmt(
                                         eat(tokens).value, STRING)));
      else {
        printf("ERROR: Unknown type '%s' in function parameters.\n",
               peek(tokens).value);
        exit(EXIT_FAILURE);
      }
    }

    else
      expect(tokens, COMMA);
  }

  expect(tokens, PAREN_R);
  return nodes;
}

NodeList *parse_func_args(TokenList *tokens, VarList *vars) {
  NodeList *nodes = create_node_list(5);

  while (peek(tokens).type != PAREN_R) {
    if (peek(tokens).type == IDENTIFIER) {
      add_node_to_node_list(
          nodes, create_expr_node(parse_additive(tokens, NULL, vars)->expr));
    }

    else if (peek(tokens).type == NUM || peek(tokens).type == STR)
      add_node_to_node_list(
          nodes,
          create_expr_node(parse_additive(tokens, gen_id(), vars)->expr));

    else {
      expect(tokens, COMMA);
    }
  }

  expect(tokens, PAREN_R);
  return nodes;
}

Node *parse_stmt(NodeList *nodes, TokenList *tokens, Env *env) {
  switch (peek(tokens).type) {
  case KEYWORD:
    if (strcmp(peek(tokens).value, "func") == 0) {
      eat(tokens);
      expect(tokens, PAREN_L);
      NodeList *params = parse_func_params(tokens);
      expect(tokens, BLOCK_START);

      NodeList *program = create_program()->program;
      while (peek(tokens).type != BLOCK_END) {
        parse_line(program, tokens, env);
        expect(tokens, NEWLINE);
      }
      expect(tokens, BLOCK_END);

      // name filled out later
      return create_stmt_node(create_func_decl_stmt("-", params, program));
    }

    else if (strcmp(peek(tokens).value, "class") == 0) {
      eat(tokens);
      if (peek(tokens).type != IDENTIFIER) {
        printf("ERROR: Expected a class name.\n");
        exit(EXIT_FAILURE);
      }
      char *class_name = eat(tokens).value;
      expect(tokens, BLOCK_START);

      NodeList *methods = create_node_list(5);
      while (peek(tokens).type != BLOCK_END && peek(tokens).type != END) {
        int first_method = methods->size;
        parse_line(methods, tokens, env);
        if (peek(tokens).type == NEWLINE)
          eat(tokens);

        if (methods->size == first_method ||
            methods->nodes[first_method].type != STMT ||
            methods->nodes[first_method].stmt->type != FUNC_DECL) {
          printf("ERROR: Class bodies currently contain named methods only.\n");
          exit(EXIT_FAILURE);
        }

        for (int i = first_method; i < methods->size; i++) {
          Node *method = &methods->nodes[i];
          if (method->type != STMT || method->stmt->type != FUNC_DECL) {
            printf("ERROR: Class bodies currently contain named methods only.\n");
            exit(EXIT_FAILURE);
          }
          char *method_name = method->stmt->func_decl->name;
          size_t size = strlen(class_name) + strlen(method_name) + 2;
          char *qualified = malloc(size);
          snprintf(qualified, size, "%s_%s", class_name, method_name);
          method->stmt->func_decl->name = qualified;
        }
      }
      expect(tokens, BLOCK_END);
      return create_stmt_node(create_class_decl_stmt(class_name, methods));
    }

    else if (strcmp(peek(tokens).value, "ret") == 0) {
      eat(tokens);
      Node *expr = parse_additive(tokens, gen_id(), env->items);
      return create_stmt_node(create_ret_stmt(expr->expr));
    }

    else if (strcmp(peek(tokens).value, "log") == 0) {
      eat(tokens);
      expect(tokens, PAREN_L);
      Node *node = parse_additive(tokens, gen_id(), env->items);

      if (node->type == STMT) {
        printf("ERROR: Expected to parse an expression but got statement "
               "instead.");
        exit(EXIT_FAILURE);
      }

      expect(tokens, PAREN_R);
      return create_stmt_node(create_log_stmt(node->expr));
    }

    else if (strcmp(peek(tokens).value, "if") == 0) {
      eat(tokens);
      expect(tokens, PAREN_L);
      Expr *left = parse_additive(tokens, NULL, env->items)->expr;
      if (peek(tokens).type != COMPARE) {
        printf("ERROR: Expected a comparison operator in if condition.\n");
        exit(EXIT_FAILURE);
      }
      char *op = eat(tokens).value;
      Expr *right = parse_additive(tokens, NULL, env->items)->expr;
      expect(tokens, PAREN_R);
      expect(tokens, BLOCK_START);

      NodeList *body = create_node_list(5);
      while (peek(tokens).type != BLOCK_END && peek(tokens).type != END) {
        parse_line(body, tokens, env);
        if (peek(tokens).type == NEWLINE)
          eat(tokens);
      }
      expect(tokens, BLOCK_END);
      return create_stmt_node(create_if_stmt(left, op, right, body));
    }

    else {
      exit(EXIT_FAILURE);
    }
    break;

  case IDENTIFIER:
    if (tokens->tokens[1].type == ASSIGNMENT) {
      char *name = eat(tokens).value;
      eat(tokens);

      NodeList *temp = parse_line(nodes, tokens, env);
      Node *rest = &temp->nodes[temp->size - 1];

      if (rest->type == EXPR) {
        create_var(name, rest->expr, env->items);
        // expr node must be at end after assignment
        remove_node_from_node_list(nodes, temp->size - 1);
        return create_stmt_node(create_assign_stmt(name, rest->expr));
      }

      else if (rest->stmt->type == FUNC_DECL) {
        rest->stmt->func_decl->name = strdup(name);
        remove_node_from_node_list(nodes, temp->size - 1);
        return rest;
      }

      remove_node_from_node_list(nodes, temp->size - 1);
      return parse_additive(tokens, NULL, env->items);
    }

    // FUNC CALL
    else if (tokens->tokens[1].type == PAREN_L) {
      char *name = eat(tokens).value;
      for (int i = 0; name[i] != '\0'; i++) {
        if (name[i] == '.')
          name[i] = '_';
      }
      expect(tokens, PAREN_L);
      NodeList *args = parse_func_args(tokens, env->items);

      return create_stmt_node(create_func_call_stmt(name, args));
    }

  default:
    return parse_additive(tokens, NULL, env->items);
  }
}
