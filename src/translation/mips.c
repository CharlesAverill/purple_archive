/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Logic for MIPS ASM generation
*/

#include "translation/mips.h"

char *mips_register_names[] = {"$t0", "$t1", "$t2", "$t3"};

void mips_preamble(FILE *fp)
{
    fputs(".data\n"
          "newline:\t.asciiz\t\"\\n\"\n"
          ".text\n"
          "main:\n"
          "\taddiu\t$fp, $sp, 0\n", // Set up stack frame
          fp);
}

void mips_postamble(FILE *fp)
{
    fputs("exit:\n"
          "\tori\t$v0, $zero, 10\n"
          "\tsyscall\n",
          fp);
}

void mips_print_newline(FILE *fp)
{
    fputs("\tli\t$v0, 4\n"
          "\tla\t$a0, newline\n"
          "\tsyscall\n",
          fp);
}

void mips_load_int(FILE *fp, int r, int value)
{
    fprintf(fp, "\tori\t%s, $zero, %d\n", mips_register_names[r], value);
}

void mips_print_int(FILE *fp, int r)
{
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
    char *flag;

    switch (mode) {
    case CMP_LT:
        flag = "slt";
        break;
    case CMP_GT:
        flag = "sgt";
        break;
    case CMP_LE:
        flag = "sle";
        break;
    case CMP_GE:
        flag = "sge";
        break;
    case CMP_EQ:
        flag = "seq";
        break;
    case CMP_NE:
        flag = "sne";
        break;
    }

    fprintf(fp, "\t%s\t%s, %s, %s\n", flag, mips_register_names[r2], mips_register_names[r1],
            mips_register_names[r2]);

    return r2;
}

void mips_create_global_variable(FILE *fp, char *identifier, int size)
{
    // MIPS does not universally support .comm, so uses the $fp register and stack offsets instead
    fprintf(fp, "\tsubu\t$sp, $sp, %d\n", size);
    return;
}

int mips_load_global_variable(FILE *fp, int r, char *identifier, int stack_offset)
{
    // Don't need the identifier
    (void)identifier;

    fprintf(fp, "\tlw\t%s, %d($fp)\n", mips_register_names[r], stack_offset);
    return r;
}

int mips_save_global_variable(FILE *fp, int r, char *identifier, int stack_offset)
{
    // Don't need the identifier
    (void)identifier;

    fprintf(fp, "\tsw\t%s, %d($fp)\n", mips_register_names[r], stack_offset);
    return r;
}
