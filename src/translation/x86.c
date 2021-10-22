/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Logic for x86 ASM generation
*/

#include "translation/x86.h"
#include "symbol_table.h"

symbol_table *symtab_stack;
//stack offset of all symbol tables EXCEPT the current top of the stack
int current_stack_offset = 0;

char *x86_register_names[] = {"%r8", "%r9", "%r10", "%r11"};
char *x86_byte_register_names[] = {"%r8b", "%r9b", "%r10b", "%r11b"};

void x86_data_section(FILE *fp) {
    fputs("\t.data\n", fp);

    for (int i = 0; i < D_GLOBAL_SYMBOL_TABLE->cur_length; i++) {
        symbol sym = D_GLOBAL_SYMBOL_TABLE->symbols[i];
        fprintf(fp, "%s:\n", sym.name);
        fprintf(fp, "\t.zero %d\n", sym.size);
    }
}

void x86_preamble(FILE *fp)
{
    symtab_stack = D_GLOBAL_SYMBOL_TABLE;

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

void x86_enter_scope(FILE *fp, symbol_table *symtab) {
    if(symtab->parent != symtab_stack) {
        fprintf(stderr, "x86 enter scope which is not child of existing scope");
        shutdown(1);
    }
    current_stack_offset += symtab->stack_offset;
    symtab_stack = symtab;
}

void x86_leave_scope(FILE *fp) {
    current_stack_offset -= symtab_stack->stack_offset;
    symtab_stack = symtab_stack->parent;
}

void x86_load_variable(FILE *fp, int r, char *identifier)
{
    symbol *symbol = get_symbol(symtab_stack, identifier);
    //global variable
    if(symbol->stack_offset == -1) {
        fprintf(fp, "\tmovq\t%s(\%%rip), %s\n", identifier, x86_register_names[r]);
    }
    //local variable
    else {
        fprintf(fp, "\tmovq\t-%d(\%%rbp), %s\n", symbol->stack_offset, x86_register_names[r]);
    }
}

void x86_save_variable(FILE *fp, int r, char *identifier)
{
    symbol *symbol = get_symbol(symtab_stack, identifier);
    //global variable
    if(symbol->stack_offset == -1) {
        fprintf(fp, "\tmovq\t%s, %s(\%%rip)\n", x86_register_names[r], identifier);
    }
    //local variable
    else {
        fprintf(fp, "\tmovq\t%s, -%d(\%%rbp)\n", x86_register_names[r], symbol->stack_offset);
    }
}

void x86_label(FILE *fp, int label_index) { fprintf(fp, "L%d:\n", label_index); }

void x86_jump_to_label(FILE *fp, int label_index) { fprintf(fp, "\tjmp\tL%d\n", label_index); }
