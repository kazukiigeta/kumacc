#include "kumacc.h"

static char *reg(int idx) {
  static char *r[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
  if (idx < 0 || sizeof(r) / sizeof(*r) <= idx) {
    error("register out of range: %d", idx);
  }
  return r[idx];
}

static int top;

static void gen_expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  mov %s, %ld\n", reg(top++), node->val);
    return;
  }

  gen_expr(node->lhs);
  gen_expr(node->rhs);

  char *rd = reg(top - 2);
  char *rs = reg(top - 1);
  top--;

  switch (node->kind) {
  case ND_ADD:
    printf("  add %s, %s\n", rd, rs);
    return;
  case ND_SUB:
    printf("  sub %s, %s\n", rd, rs);
    return;
  case ND_MUL:
    printf("  imul %s, %s\n", rd, rs);
    return;
  case ND_DIV:
    printf("  mov rax, %s\n", rd);
    printf("  cqo\n");
    printf("  idiv %s\n", rs);
    printf("  mov %s, rax\n", rd);
    return;
  case ND_EQ:
    printf("  cmp %s, %s\n", rd, rs);
    printf("  sete al\n");
    printf("  movzb %s, al\n", rd);
    return;
  case ND_NE:
    printf("  cmp %s, %s\n", rd, rs);
    printf("  setne al\n");
    printf("  movzb %s, al\n", rd);
    return;
  case ND_LT:
    printf("  cmp %s, %s\n", rd, rs);
    printf("  setl al\n");
    printf("  movzb %s, al\n", rd);
    return;
  case ND_LE:
    printf("  cmp %s, %s\n", rd, rs);
    printf("  setle al\n");
    printf("  movzb %s, al\n", rd);
    return;
  default:
    error("invalid expression");
  }
}

void codegen(Node *node) {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push r12\n");
  printf("  push r13\n");
  printf("  push r14\n");
  printf("  push r15\n");

  gen_expr(node);

  printf("  mov rax, %s\n", reg(top - 1));

  printf("  pop r15\n");
  printf("  pop r14\n");
  printf("  pop r13\n");
  printf("  pop r12\n");
  printf("  ret\n");
}