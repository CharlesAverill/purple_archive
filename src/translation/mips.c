/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Logic for MIPS ASM generation
*/

#include "translation/mips.h"

static symbol_table *symtab_stack;
// Stack offset of all symbol tables EXCEPT the current top of the stack
static int current_stack_offset = 0;

char *mips_register_names[] = {"$t0", "$t1", "$t2", "$t3"};

void mips_data_section(FILE *fp)
{
    fputs(".data\n"
          "newline:\t.asciiz\t\"\\n\"\n",
          fp);

    for (int i = 0; i < D_GLOBAL_SYMBOL_TABLE->cur_length; i++) {
        symbol sym = D_GLOBAL_SYMBOL_TABLE->symbols[i];

        if (sym.size <= 32) {
            fprintf(fp, "%s:\t.word\t0", sym.name);
        } else if (sym.size <= 64) {
            fprintf(fp, "%s:\t.double\t0", sym.name);
        } else {
            fprintf(stderr, "Symbol %s requires too much space - %d bytes\n", sym.name, sym.size);
            shutdown(1);
        }

        // Use .space to offset anything that doesn't end on a word (4 byte) boundary
        int remainder = sym.size % 4;
        if (remainder != 0) {
            fprintf(fp, "\t.space\t%d", remainder);
        }

        fprintf(fp, "\n");
    }
}

void mips_preamble(FILE *fp)
{
    fputs(".text\n"
          "main:\n"
          "\taddiu\t$fp, $sp, 0\n" // Set up stack frame
          "\n",
          fp);
}

void mips_postamble(FILE *fp)
{
    fputs("\n"
          "exit:\n"
          "\tori\t$v0, $zero, 10\n"
          "\tsyscall\n",
          fp);
}

void mips_print_newline(FILE *fp)
{
    fputs("\tori\t$v0, $zero, 4\n"
          "\tla\t$a0, newline\n"
          "\tsyscall\n"
          "\n",
          fp);
}

void mips_load_int(FILE *fp, int r, int value)
{
    fprintf(fp, "\tori\t%s, $zero, %d\n", mips_register_names[r], value);
}

void mips_print_int(FILE *fp, int r)
{
    fputs("\n", fp);
    fprintf(fp, "\tori\t$v0, $zero, 1\n");
    fprintf(fp, "\tadd\t$a0, %s, $zero\n", mips_register_names[r]);
    fprintf(fp, "\tsyscall\n");

    mips_print_newline(fp);
}

int mips_add(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tadd\t%s, %s, %s\n", mips_register_names[r1], mips_register_names[r1],
            mips_register_names[r2]);
    return r1;
}

int mips_sub(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tsub\t%s, %s, %s\n", mips_register_names[r1], mips_register_names[r1],
            mips_register_names[r2]);
    return r1;
}

int mips_mul(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tmult\t%s, %s\n", mips_register_names[r1], mips_register_names[r2]);
    fprintf(fp, "\tmflo\t%s\n", mips_register_names[r2]);
    return r2;
}

int mips_div(FILE *fp, int r1, int r2)
{
    fprintf(fp, "\tdiv\t%s, %s\n", mips_register_names[r1], mips_register_names[r2]);
    fprintf(fp, "\tmflo\t%s\n", mips_register_names[r2]);
    return r2;
}

int mips_compare(FILE *fp, int r1, int r2, Comparison_Mode mode)
{
    // MIPS typically only supports SLT
    switch (mode) {
    case CMP_LT:
        fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1],
                mips_register_names[r2]);
        break;
    case CMP_GT:
        fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r2],
                mips_register_names[r1]);
        break;
    case CMP_LE:
        fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r2],
                mips_register_names[r1]);
        fprintf(fp, "\tori\t$at, $zero, 1\n");
        fprintf(fp, "\tsubu\t%s, $at, %s\n", mips_register_names[r2], mips_register_names[r2]);
        break;
    case CMP_GE:
        fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1],
                mips_register_names[r2]);
        fprintf(fp, "\tori\t$at, $zero, 1\n");
        fprintf(fp, "\tsubu\t%s, $at, %s\n", mips_register_names[r2], mips_register_names[r2]);
        break;
    case CMP_EQ:
        fprintf(fp, "\tsubu\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1],
                mips_register_names[r2]);
        fprintf(fp, "\tori\t$at, $zero, 1\n");
        fprintf(fp, "\tsltu\t%s, %s, $at\n", mips_register_names[r2], mips_register_names[r2]);
        break;
    case CMP_NE:
        fprintf(fp, "\tsubu\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1],
                mips_register_names[r2]);
        fprintf(fp, "\tsltu\t%s, $zero, %s\n", mips_register_names[r2], mips_register_names[r2]);
        break;
    }

    return r2;
}

int mips_compare_and_jump(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index)
{
    // MIPS typically only supports BEQ and BNE
    switch (mode) {
    case CMP_LT:
        fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r1], mips_register_names[r2]);
        fprintf(fp, "\tbeq\t$at, $zero, L%d\n", label_index);
        break;
    case CMP_GT:
        fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r2], mips_register_names[r1]);
        fprintf(fp, "\tbeq\t$at, $zero, L%d\n", label_index);
        break;
    case CMP_LE:
        fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r2], mips_register_names[r1]);
        fprintf(fp, "\tbne\t$at, $zero, L%d\n", label_index);
        break;
    case CMP_GE:
        fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r1], mips_register_names[r2]);
        fprintf(fp, "\tbne\t$at, $zero, L%d\n", label_index);
        break;
    case CMP_EQ:
        fprintf(fp, "\tbne\t%s, %s, L%d\n", mips_register_names[r1], mips_register_names[r2],
                label_index);
        break;
    case CMP_NE:
        fprintf(fp, "\tbeq\t%s, %s, L%d\n", mips_register_names[r1], mips_register_names[r2],
                label_index);
        break;
    }

    return NO_REGISTER;
}

void mips_enter_scope(FILE *fp, symbol_table *symtab)
{
    if (symtab_stack == NULL) {
        symtab_stack = symtab;
        return;
    }
    if (symtab->parent != symtab_stack) {
        fprintf(stderr, "MIPS enter scope which is not child of existing scope\n");
        shutdown(1);
    }

    current_stack_offset += symtab->stack_offset;
    symtab_stack = symtab;
}

void mips_leave_scope(FILE *fp)
{
    current_stack_offset -= symtab_stack->stack_offset;
    symtab_stack = symtab_stack->parent;
}

void mips_load_variable(FILE *fp, int r, char *identifier)
{
    symbol *symbol = get_symbol(symtab_stack, identifier);
    if (symbol->stack_offset == -1) {
        // Global variable, load from .data
        fprintf(fp, "\tlw\t%s, %s\n", mips_register_names[r], identifier);
    } else {
        // Local variable, load from stack
        fprintf(fp, "\tlw\t%s, %d($fp)\n", mips_register_names[r],
                symbol->stack_offset + current_stack_offset);
    }
}

void mips_save_variable(FILE *fp, int r, char *identifier)
{
    symbol *symbol = get_symbol(symtab_stack, identifier);
    if (symbol->stack_offset == -1) {
        // Global variable, save to .data
        fprintf(fp, "\tsw\t%s, %s\n", mips_register_names[r], identifier);
    } else {
        // Local variable, save to stack
        fprintf(fp, "\tsw\t%s, %d($fp)\n", mips_register_names[r],
                symbol->stack_offset + current_stack_offset);
    }
}

void mips_label(FILE *fp, int label_index) { fprintf(fp, "L%d:\n", label_index); }

void mips_jump_to_label(FILE *fp, int label_index) { fprintf(fp, "\tj\tL%d\n", label_index); }
