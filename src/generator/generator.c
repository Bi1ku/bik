#include "../../include/generator/eval.h"
#include "../../include/parser/ast.h"
#include "../../include/parser/env.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int t_reg = 0;
int f_reg = 0;
int a_reg = 0;

void generate_from_nodes(FILE *out, Env *env, NodeList *program);

void generate_expr(FILE *out, Expr *expr, Env *env, bool ret, char *var_name) {
  switch (expr->type) {
  case INT:
    if (ret) {
      if (var_name != NULL)
        fprintf(out, "\tli $v0, %d\n", expr->integer);
    } else {
      fprintf(out, "\tli $t%d, %d\n", t_reg, expr->integer);
      t_reg++;
    }

    break;

  case FLOAT:
    if (var_name) {
      fprintf(out, "\tl.s $f%d, %s\n", f_reg, var_name);
      f_reg++;
    }
    break;

  case IDENTIFIER_EX: {
    Expr *var = get_var(env->items, expr->identifier);
    generate_expr(out, var, env, ret, expr->identifier);
    break;
  }

  case BIN: {
    Expr *res = eval(expr->bin, env->items);
    generate_expr(out, res, env, ret, NULL);
    break;
  }

  case STRING:
    if (var_name) {
      ret ? fprintf(out, "\tla $v0, %s\n", var_name)
          : fprintf(out, "\tla $t%d, %s\n", t_reg, var_name);
    } else {
      printf("ERROR: Haven't done string asm logic yet");
      exit(EXIT_FAILURE);
    }

  default:
    printf("ERROR: Unsure how to generate asm with expr type %d", expr->type);
    exit(EXIT_FAILURE);
  }
}

void generate_log(FILE *out, Expr *expr, Env *env, char *var_name) {
  switch (expr->type) {
  case STRING:
    if (var_name) {
      fprintf(out, "\tli $v0, 4\n");
      fprintf(out, "\tla $a0, %s\n", var_name);
    }
    fprintf(out, "\tsyscall\n");
    break;

  case INT:
    fprintf(out, "\tli $v0, 1\n");
    fprintf(out, "\tli $a0, %d\n", expr->integer);
    fprintf(out, "\tsyscall\n");
    break;

  case FLOAT:
    if (var_name) {
      fprintf(out, "\tli $v0, 2\n");
      fprintf(out, "\tlwc1 $f12, %s\n", var_name);
    }
    fprintf(out, "\tsyscall\n");
    break;

  case IDENTIFIER_EX: {
    Expr *var = get_var(env->items, expr->identifier);
    generate_log(out, var, env, expr->identifier);
    break;
  }

  case BIN: {
    Expr *ans = eval(expr->bin, env->items);
    generate_log(out, ans, env, NULL);
    break;
  }
  }
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
  case FUNC:
    fprintf(out, "%s:\n", stmt->func->name);
    generate_from_nodes(out, env, stmt->func->body);
    break;

  case RET:
    generate_expr(out, stmt->ret, env, true, NULL);
    fprintf(out, "\tjr $ra\n");
    break;

  case LOG:
    generate_log(out, stmt->log, env, NULL);
    break;

  case ASSIGN: // already handled in generate_vars
    break;

  default:
    printf("ERROR: Unsure how to generate asm with stmt type %d", stmt->type);
    exit(EXIT_FAILURE);
  }
}

void generate_vars(FILE *out, Env *env) {
  for (int i = 0; i < env->items->size; i++) {
    Expr *value = env->items->items[i]->value;
    char *name = env->items->items[i]->key;

    if (name != NULL) {
      switch (value->type) {
      case STRING:
        fprintf(out, "\t%s: .asciiz \"%s\"\n", name, value->str);
        break;

      case INT:
        fprintf(out, "\t%s: .word %d\n", name, value->integer);
        break;

      case FLOAT:
        fprintf(out, "\t%s: .float %lf\n", name, value->floating);
        break;

      default:
        printf("ERROR: Not Valid!");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void generate_funcs(FILE *out, Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.stmt->type == FUNC)
      generate_stmt(out, node.stmt, env);
  }
}

void generate_from_nodes(FILE *out, Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.type == STMT && node.stmt->type != FUNC)
      generate_stmt(out, node.stmt, env);
    else if (node.type == EXPR)
      generate_expr(out, node.expr, env, false, NULL);
  }
}

void generate(FILE *out, Env *env, NodeList *program) {
  fprintf(out, ".data\n");
  generate_vars(out, env);

  fprintf(out, "\n.text\n");
  fprintf(out, ".globl main\n");
  fprintf(out, "main:\n");
  generate_from_nodes(out, env, program);
  fprintf(out, "\tli $v0, 10\n");
  fprintf(out, "\tsyscall\n");
  generate_funcs(out, env, program);

  fclose(out);
}
