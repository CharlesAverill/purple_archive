/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Logic for x86 ASM generation
*/

#include "translation/x86.h"

char *x86_register_names[] = {"%r8", "%r9", "%r10", "%r11"};
char *x86_byte_register_names[] = {"%r8b", "%r9b", "%r10b", "%r11b"};

void x86_preamble(FILE *fp)
{
    fputs("\t.text\n"
          ".LC0:\n"
          "\t.string\t\"%d\\n\"\n"
          "print_int:\n"
          "\tpushq\t%rbp\n"
          "\tmovq\t%rsp, %rbp\n"
          "\tsubq\t$16, %rsp\n"
          "\tmovl\t%edi, -4(%rbp)\n"
          "\tmovl\t-4(%rbp), %eax\n"
          "\tmovl\t%eax, %esi\n"
          "\tleaq	.LC0(%rip), %rdi\n"
          "\tmovl	$0, %eax\n"
          "\tcall	printf@PLT\n"
          "\tnop\n"
          "\tleave\n"
          "\tret\n"
          "\n"
          "\t.globl\tmain\n"
          "\t.type\tmain, @function\n"
          "main:\n"
          "\tpushq\t%rbp\n"
          "\tmovq	%rsp, %rbp\n"
          "\n",
          fp);
}

void x86_postamble(FILE *fp)
{
    fputs("\n"
          "\tmovl	$0, %eax\n"
          "\tpopq	%rbp\n"
          "\tret\n",
          fp);
}

void x86_load_int(FILE *fp, int r, int value)
{
    fprintf(fp, "\tmovq\t$%d, %s\n", value, x86_register_names[r]);
}

void x86_print_int(FILE *fp, int r)
{
    fprintf(fp, "\tmovq\t%s, %%rdi\n", x86_register_names[r]);
    fprintf(fp, "\tcall\tprint_int\n");
}

int x86_add(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\taddq\t%s, %s\n", x86_register_names[r1], x86_register_names[r2]);
    return r2;
}

int x86_sub(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tsubq\t%s, %s\n", x86_register_names[r2], x86_register_names[r1]);
    return r1;
}

int x86_mul(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\timulq\t%s, %s\n", x86_register_names[r1], x86_register_names[r2]);
    return r2;
}

int x86_div(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tmovq\t%s, %%rax\n", x86_register_names[r1]);
    fprintf(fp, "\tcqo\n");
    fprintf(fp, "\tidivq\t%s\n", x86_register_names[r2]);
    fprintf(fp, "\tmovq\t%%rax, %s\n", x86_register_names[r1]);
    return r1;
}

char *flag_setters[] = {"setl", "setle", "setg", "setge", "sete", "setne"};
int x86_compare(FILE *fp, int r1, int r2, Comparison_Mode mode)
{
    // Compare
    fprintf(fp, "\tcmpq\t%s, %s\n", x86_register_names[r2], x86_register_names[r1]);
    // Only read lower 8 bits (1 byte) of register for comparison
    fprintf(fp, "\t%s\t%s\n", flag_setters[mode], x86_byte_register_names[r2]);
    // And with 255 to ensure no bits leftover from previous operations
    fprintf(fp, "\tmovzbq\t%s,%s\n", x86_byte_register_names[r2], x86_register_names[r2]);
    return r2;
}

char *jump_comparisons[] = {"jle", "jl", "jge", "jg", "jne", "je"};
int x86_compare_and_jump(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index)
{
    fprintf(fp, "\tcmpq\t%s, %s\n", x86_register_names[r1], x86_register_names[r2]);
    fprintf(fp, "\t%s\tL%d\n", jump_comparisons[mode], label_index);
    return NO_REGISTER;
}

void x86_create_global_variable(FILE *fp, char *identifier, int size)
{
    // x86 universally supports .comm, so does not use stack offsets
    fprintf(fp, "\t.comm\t%s,8,8\n", identifier);
}

int x86_load_global_variable(FILE *fp, int r, char *identifier, int stack_offset)
{
    // Don't need the stack offset
    (void)stack_offset;

    fprintf(fp, "\tmovq\t%s(\%%rip), %s\n", identifier, x86_register_names[r]);
    return r;
}

int x86_save_global_variable(FILE *fp, int r, char *identifier, int stack_offset)
{
    // Don't need the stack offset
    (void)stack_offset;

    fprintf(fp, "\tmovq\t%s, %s(\%%rip)\n", x86_register_names[r], identifier);
    return r;
}

void x86_label(FILE *fp, int label_index) { fprintf(fp, "L%d:\n", label_index); }

void x86_jump_to_label(FILE *fp, int label_index) { fprintf(fp, "\tjmp\tL%d\n", label_index); }
