#include "../../include/generator/eval.h"
#include "../../include/generator/expr.h"
#include "../../include/generator/generator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parameter_index(const char *name) {
  if (current_func == NULL)
    return -1;

  for (int i = 0; i < current_func->params->size; i++) {
    Param *param = current_func->params->nodes[i].stmt->param;
    if (strcmp(param->name, name) == 0)
      return i;
  }
  return -1;
}

static void generate_epilogue(FILE *out, FuncDecl *func) {
  int frame_size = 4 * (func->params->size + 1);
  for (int i = 0; i < func->params->size && i < 4; i++)
    fprintf(out, "\tlw $a%d, %d($sp)\n", i, 4 * (i + 1));
  fprintf(out, "\tlw $ra, 0($sp)\n");
  fprintf(out, "\taddiu $sp, $sp, %d\n", frame_size);
  fprintf(out, "\tjr $ra\n");
}

void generate_log(FILE *out, Expr *expr, Env *env, char *var_name) {
  switch (expr->type) {
  case STRING:
    fprintf(out, "\tli $v0, 4\n");
    var_name ? fprintf(out, "\tla $a0, %s\n", var_name)
             : fprintf(out, "\tla $a0, %s\n", expr->str->mem);
    fprintf(out, "\tsyscall\n");
    break;

  case INT:
    fprintf(out, "\tli $v0, 1\n");
    fprintf(out, "\tli $a0, %d\n", expr->integer);
    fprintf(out, "\tsyscall\n");
    break;

  case FLOAT:
    fprintf(out, "\tli $v0, 2\n");
    var_name ? fprintf(out, "\tlwc1 $f12, %s\n", var_name)
             : fprintf(out, "\tlwc1 $f12, %s\n", expr->floating->mem);
    fprintf(out, "\tsyscall\n");
    break;

  case IDENTIFIER_EX: {
    int index = parameter_index(expr->identifier);
    if (index >= 0) {
      fprintf(out, "\tli $v0, 1\n");
      fprintf(out, "\tmove $a0, $a%d\n", index);
      fprintf(out, "\tsyscall\n");
      break;
    }
    Expr *var = get_var(env->items, expr->identifier);
    generate_log(out, var, env, expr->identifier);
    break;
  }

  case BIN: {
    generate_int_expr(out, expr, env, 0);
    fprintf(out, "\tli $v0, 1\n");
    fprintf(out, "\tmove $a0, $t0\n");
    fprintf(out, "\tsyscall\n");
    break;
  }
  }
}

static void generate_args(FILE *out, Expr *arg, Env *env, int index) {
  if (index >= 4) {
    fprintf(stderr, "ERROR: Functions support at most four arguments.\n");
    exit(EXIT_FAILURE);
  }
  generate_int_expr(out, arg, env, 0);
  fprintf(out, "\tmove $a%d, $t0\n", index);
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
  case FUNC_DECL:
    if (stmt->func_decl->params->size > 4) {
      fprintf(stderr, "ERROR: Functions support at most four parameters.\n");
      exit(EXIT_FAILURE);
    }
    current_func = stmt->func_decl;
    fprintf(out, "%s:\n", stmt->func_decl->name);
    fprintf(out, "\taddiu $sp, $sp, -%d\n",
            4 * (stmt->func_decl->params->size + 1));
    fprintf(out, "\tsw $ra, 0($sp)\n");
    for (int i = 0; i < stmt->func_decl->params->size && i < 4; i++)
      fprintf(out, "\tsw $a%d, %d($sp)\n", i, 4 * (i + 1));
    generate_from_nodes(out, env, stmt->func_decl->body);
    generate_epilogue(out, stmt->func_decl);
    current_func = NULL;
    break;

  case FUNC_CALL:
    for (int i = 0; i < stmt->func_call->args->size; i++) {
      Expr *arg = stmt->func_call->args->nodes[i].expr;

      generate_args(out, arg, env, i);
    }

    fprintf(out, "\tjal %s\n", stmt->func_call->name);
    if (current_func != NULL) {
      for (int i = 0; i < current_func->params->size && i < 4; i++)
        fprintf(out, "\tlw $a%d, %d($sp)\n", i, 4 * (i + 1));
    }
    break;

  case RET:
    generate_expr(out, stmt->ret, env, true, NULL);
    if (current_func != NULL)
      generate_epilogue(out, current_func);
    else
      fprintf(out, "\tjr $ra\n");
    break;

  case LOG:
    generate_log(out, stmt->log, env, NULL);
    break;

  case ASSIGN: // already handled in generate_vars
    break;

  case IF: {
    int label = label_index++;
    generate_int_expr(out, stmt->if_stmt->left, env, 0);
    generate_int_expr(out, stmt->if_stmt->right, env, 1);
    const char *branch = NULL;
    if (strcmp(stmt->if_stmt->op, "==") == 0) branch = "bne";
    else if (strcmp(stmt->if_stmt->op, "!=") == 0) branch = "beq";
    else if (strcmp(stmt->if_stmt->op, "<") == 0) branch = "bge";
    else if (strcmp(stmt->if_stmt->op, ">") == 0) branch = "ble";
    else if (strcmp(stmt->if_stmt->op, "<=") == 0) branch = "bgt";
    else if (strcmp(stmt->if_stmt->op, ">=") == 0) branch = "blt";
    if (branch == NULL) {
      fprintf(stderr, "ERROR: Unsupported comparison '%s'.\n", stmt->if_stmt->op);
      exit(EXIT_FAILURE);
    }
    fprintf(out, "\t%s $t0, $t1, if_end_%d\n", branch, label);
    generate_from_nodes(out, env, stmt->if_stmt->body);
    fprintf(out, "if_end_%d:\n", label);
    break;
  }

  default:
    printf("ERROR: Unsure how to generate asm with stmt type %d", stmt->type);
    exit(EXIT_FAILURE);
  }
}

void generate_funcs(FILE *out, Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.type == STMT && node.stmt->type == FUNC_DECL)
      generate_stmt(out, node.stmt, env);
  }
}
