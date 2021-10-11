#include "translation/x86.h"

char *x86_register_names[] = {"%r8", "%r9", "%r10", "%r11"};

void x86_preamble(FILE *fp){
    fputs(
        "\t.text\n"
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

void x86_postamble(FILE *fp){
    fputs(
        "\n"
        "\tmovl	$0, %eax\n"
        "\tpopq	%rbp\n"
        "\tret\n",
    fp);
}

void x86_load(FILE *fp, int r, int value){
    fprintf(fp, "\tmovq\t$%d, %s\n", value, x86_register_names[r]);
}

void x86_print_int(FILE *fp, int r){
    fprintf(fp, "\tmovq\t%s, %%rdi\n", x86_register_names[r]);
    fprintf(fp, "\tcall\tprint_int\n");
}

int x86_add(FILE *fp, int r1, int r2){
    fprintf(fp, "\taddq\t%s, %s\n", x86_register_names[r1], x86_register_names[r2]);
    return r2;
}

int x86_sub(FILE *fp, int r1, int r2){
    fprintf(fp, "\tsubq\t%s, %s\n", x86_register_names[r2], x86_register_names[r1]);
    return r1;
}

int x86_mul(FILE *fp, int r1, int r2){
    fprintf(fp, "\timulq\t%s, %s\n", x86_register_names[r1], x86_register_names[r2]);
    return r2;
}

int x86_div(FILE *fp, int r1, int r2){
    fprintf(fp, "\tmovq\t%s, %%rax\n", x86_register_names[r1]);
    fprintf(fp, "\tcqo\n");
    fprintf(fp, "\tidivq\t%s\n", x86_register_names[r2]);
    fprintf(fp, "\tmovq\t%%rax, %s\n", x86_register_names[r1]);
    return r1;
}
