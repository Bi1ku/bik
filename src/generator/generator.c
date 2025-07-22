#include "../../include/generator/expr.h"
#include "../../include/generator/stmt.h"
#include "../../include/generator/vars.h"

int t_reg = 0;
int f_reg = 0;
int a_reg = 0;

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
