#include "../../include/generator/eval.h"
#include "../../include/generator/expr.h"
#include "../../include/generator/generator.h"
#include <stdbool.h>
#include <stdlib.h>

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

void generate_args(FILE *out, Expr *arg, Env *env, char *var_name) {
  switch (arg->type) {
  case INT:
    fprintf(out, "\tli $a%d, %d\n", a_reg, arg->integer);
    a_reg++;
    break;

  case FLOAT:
    var_name ? fprintf(out, "\tlwc1 $f%d, %s\n", f_reg, var_name)
             : fprintf(out, "\tlwc1 $f%d, %s\n", f_reg, arg->floating->mem);
    f_reg++;
    break;

  case IDENTIFIER_EX: {
    Expr *var = get_var(env->items, arg->identifier);
    generate_args(out, var, env, arg->identifier);
    break;
  }

  case BIN: {
    Expr *res = eval(arg->bin, env->items);
    generate_args(out, res, env, NULL);
    break;
  }

  default:
    printf("ERROR: Unsure how to generate asm with arg type %d", arg->type);
    exit(EXIT_FAILURE);
  }
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
  case FUNC_DECL:
    fprintf(out, "%s:\n", stmt->func_decl->name);
    generate_from_nodes(out, env, stmt->func_decl->body);
    break;

  case FUNC_CALL:
    for (int i = 0; i < stmt->func_call->args->size; i++) {
      Expr *arg = stmt->func_call->args->nodes[i].expr;

      generate_args(out, arg, env, NULL);
    }

    fprintf(out, "\tjal %s\n", stmt->func_call->name);
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

void generate_funcs(FILE *out, Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node node = program->nodes[i];

    if (node.stmt->type == FUNC_DECL)
      generate_stmt(out, node.stmt, env);
  }
}
