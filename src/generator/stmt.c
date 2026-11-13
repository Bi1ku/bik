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

static int frame_size(FuncDecl *func) {
  return (4 * (func->params->size + 2) + 7) / 8 * 8;
}

static void generate_epilogue(FILE *out, FuncDecl *func) {
  fprintf(out, "\tmove $sp, $fp\n");
  fprintf(out, "\tlw $ra, 0($sp)\n\tlw $fp, 4($sp)\n");
  fprintf(out, "\taddiu $sp, $sp, %d\n", frame_size(func));
  fprintf(out, "\tjr $ra\n\tnop\n");
}

void generate_log(FILE *out, Expr *expr, Env *env, char *var_name) {
  switch (expr->type) {
  case STRING:
    fprintf(out, "\tli $v0, 4\n");
    var_name ? fprintf(out, "\tla $a0, __bik_var_%s\n", var_name)
             : fprintf(out, "\tla $a0, __bik_var_%s\n", expr->str->mem);
    fprintf(out, "\tsyscall\n");
    break;

  case INT:
    fprintf(out, "\tli $v0, 1\n");
    fprintf(out, "\tli $a0, %d\n", expr->integer);
    fprintf(out, "\tsyscall\n");
    break;

  case FLOAT:
    fprintf(out, "\tli $v0, 2\n");
    var_name ? fprintf(out, "\tlwc1 $f12, __bik_var_%s\n", var_name)
             : fprintf(out, "\tlwc1 $f12, __bik_var_%s\n", expr->floating->mem);
    fprintf(out, "\tsyscall\n");
    break;

  case IDENTIFIER_EX: {
    int index = parameter_index(expr->identifier);
    Expr *var = get_var(env->items, expr->identifier);
    if (index >= 0 || (var != NULL && var->type == INT)) {
      generate_int_expr(out, expr, env, 0);
      fprintf(out, "\tli $v0, 1\n\tmove $a0, $t0\n\tsyscall\n");
      break;
    }
    if (var == NULL) {
      fprintf(stderr, "ERROR: Unknown variable '%s'.\n", expr->identifier);
      exit(EXIT_FAILURE);
    }
    generate_log(out, var, env, expr->identifier);
    break;
  }

  case CALL:
  case BIN: {
    generate_int_expr(out, expr, env, 0);
    fprintf(out, "\tli $v0, 1\n");
    fprintf(out, "\tmove $a0, $t0\n");
    fprintf(out, "\tsyscall\n");
    break;
  }

  case OBJECT:
    fprintf(stderr, "ERROR: Objects cannot be logged directly; select a field.\n");
    exit(EXIT_FAILURE);
  }
}

void collect_funcs(Env *env, NodeList *program) {
  for (int i = 0; i < program->size; i++) {
    Node *node = &program->nodes[i];
    if (node->type != STMT)
      continue;
    if (node->stmt->type == CLASS_DECL) {
      collect_funcs(env, node->stmt->class_decl->methods);
    } else if (node->stmt->type == FUNC_DECL) {
      char *name = node->stmt->func_decl->name;
      for (int j = 0; j < env->functions->size; j++) {
        if (strcmp(env->functions->nodes[j].stmt->func_decl->name, name) == 0) {
          fprintf(stderr, "ERROR: Duplicate function '%s'.\n", name);
          exit(EXIT_FAILURE);
        }
      }
      add_node_to_node_list(env->functions, node);
    }
  }
}

void generate_call(FILE *out, FuncCall *call, Env *env) {
  char *name = call->name;
  size_t prefix = 0;
  for (int i = 0; i < env->items->size; i++) {
    Var *var = env->items->items[i];
    size_t length = strlen(var->key);
    if (var->value->type == OBJECT && length > prefix &&
        strncmp(call->name, var->key, length) == 0 && call->name[length] == '_') {
      char *class_name = var->value->object->class_decl->name;
      size_t size = strlen(class_name) + strlen(call->name + length) + 1;
      if (name != call->name)
        free(name);
      name = malloc(size);
      snprintf(name, size, "%s%s", class_name, call->name + length);
      prefix = length;
    }
  }
  FuncDecl *func = NULL;
  for (int i = 0; i < env->functions->size; i++) {
    FuncDecl *candidate = env->functions->nodes[i].stmt->func_decl;
    if (strcmp(candidate->name, name) == 0) {
      func = candidate;
      break;
    }
  }
  if (func == NULL) {
    fprintf(stderr, "ERROR: Unknown function '%s'.\n", call->name);
    exit(EXIT_FAILURE);
  }
  if (call->args->size > 4) {
    fprintf(stderr, "ERROR: Functions support at most four arguments.\n");
    exit(EXIT_FAILURE);
  }
  if (call->args->size != func->params->size) {
    fprintf(stderr, "ERROR: Function '%s' expects %d arguments, got %d.\n",
            call->name, func->params->size, call->args->size);
    exit(EXIT_FAILURE);
  }

  int size = (4 * call->args->size + 7) / 8 * 8;
  if (size > 0)
    fprintf(out, "\taddiu $sp, $sp, -%d\n", size);
  for (int i = 0; i < call->args->size; i++) {
    generate_int_expr(out, call->args->nodes[i].expr, env, 0);
    fprintf(out, "\tsw $t0, %d($sp)\n", 4 * i);
  }
  for (int i = 0; i < call->args->size; i++)
    fprintf(out, "\tlw $a%d, %d($sp)\n", i, 4 * i);
  fprintf(out, "\tjal __bik_func_%s\n\tnop\n", name);
  if (size > 0)
    fprintf(out, "\taddiu $sp, $sp, %d\n", size);
  if (name != call->name)
    free(name);
}

void generate_stmt(FILE *out, Stmt *stmt, Env *env) {
  switch (stmt->type) {
  case FUNC_DECL:
    if (stmt->func_decl->params->size > 4) {
      fprintf(stderr, "ERROR: Functions support at most four parameters.\n");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < stmt->func_decl->params->size; i++) {
      if (stmt->func_decl->params->nodes[i].stmt->param->type != INT) {
        fprintf(stderr, "ERROR: Function parameters must be integers.\n");
        exit(EXIT_FAILURE);
      }
    }
    current_func = stmt->func_decl;
    fprintf(out, "__bik_func_%s:\n", current_func->name);
    fprintf(out, "\taddiu $sp, $sp, -%d\n", frame_size(current_func));
    fprintf(out, "\tsw $ra, 0($sp)\n\tsw $fp, 4($sp)\n\tmove $fp, $sp\n");
    for (int i = 0; i < current_func->params->size; i++)
      fprintf(out, "\tsw $a%d, %d($fp)\n", i, 8 + 4 * i);
    generate_from_nodes(out, env, current_func->body);
    fprintf(out, "\tli $v0, 0\n");
    generate_epilogue(out, current_func);
    current_func = NULL;
    break;

  case FUNC_CALL:
    generate_call(out, stmt->func_call, env);
    break;

  case RET:
    if (current_func == NULL) {
      fprintf(stderr, "ERROR: Return outside a function.\n");
      exit(EXIT_FAILURE);
    }
    generate_int_expr(out, stmt->ret, env, 0);
    fprintf(out, "\tmove $v0, $t0\n");
    generate_epilogue(out, current_func);
    break;

  case LOG:
    generate_log(out, stmt->log, env, NULL);
    break;

  case ASSIGN: {
    Expr *value = get_var(env->items, stmt->assign->symbol);
    if (stmt->assign->expr->type == OBJECT) {
      NodeList *fields = stmt->assign->expr->object->class_decl->fields;
      for (int i = 0; i < fields->size; i++) {
        Assign *field = fields->nodes[i].stmt->assign;
        if (field->expr->type == INT) {
          fprintf(out, "\tli $t0, %d\n", field->expr->integer);
          fprintf(out, "\tsw $t0, __bik_var_%s_%s\n",
                  stmt->assign->symbol, field->symbol);
        }
      }
    } else if (value != NULL && value->type == INT) {
      generate_int_expr(out, stmt->assign->expr, env, 0);
      fprintf(out, "\tsw $t0, __bik_var_%s\n", stmt->assign->symbol);
    }
    break;
  }

  case IF: {
    int label = label_index++;
    generate_int_expr(out, stmt->if_stmt->left, env, 0);
    fprintf(out, "\taddiu $sp, $sp, -8\n\tsw $t0, 0($sp)\n");
    generate_int_expr(out, stmt->if_stmt->right, env, 0);
    fprintf(out, "\tmove $t1, $t0\n\tlw $t0, 0($sp)\n\taddiu $sp, $sp, 8\n");
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
    fprintf(out, "\t%s $t0, $t1, if_end_%d\n\tnop\n", branch, label);
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
    else if (node.type == STMT && node.stmt->type == CLASS_DECL)
      generate_funcs(out, env, node.stmt->class_decl->methods);
  }
}
