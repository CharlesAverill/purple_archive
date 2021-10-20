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
    // MIPS typically only supports SLT
    switch(mode){
        case CMP_LT:
            fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1], mips_register_names[r2]);
            break;
        case CMP_GT:
            fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r2], mips_register_names[r1]);
            break;
        case CMP_LE:
            fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r2], mips_register_names[r1]);
            fprintf(fp, "\tori\t$at, $zero, 1\n");
            fprintf(fp, "\tsubu\t%s, $at, %s\n", mips_register_names[r2], mips_register_names[r2]);
            break;
        case CMP_GE:
            fprintf(fp, "\tslt\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1], mips_register_names[r2]);
            fprintf(fp, "\tori\t$at, $zero, 1\n");
            fprintf(fp, "\tsubu\t%s, $at, %s\n", mips_register_names[r2], mips_register_names[r2]);
            break;
        case CMP_EQ:
            fprintf(fp, "\tsubu\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1], mips_register_names[r2]);
            fprintf(fp, "\tori\t$at, $zero, 1\n");
            fprintf(fp, "\tsltu\t%s, %s, $at\n", mips_register_names[r2], mips_register_names[r2]);
            break;
        case CMP_NE:
            fprintf(fp, "\tsubu\t%s, %s, %s\n", mips_register_names[r2], mips_register_names[r1], mips_register_names[r2]);
            fprintf(fp, "\tsltu\t%s, $zero, %s\n", mips_register_names[r2], mips_register_names[r2]);
            break;
    }

    return r2;
}

int mips_compare_and_jump(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index){
    // MIPS typically only supports BEQ and BNE
    switch(mode){
        case CMP_LT:
            fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r2], mips_register_names[r1]);
            fprintf(fp, "\tbne\t$at, $zero, L%d\n", label_index);
            break;
        case CMP_GT:
            fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r1], mips_register_names[r2]);
            fprintf(fp, "\tbne\t$at, $zero, L%d\n", label_index);
            break;
        case CMP_LE:
            fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r1], mips_register_names[r2]);
            fprintf(fp, "\tbeq\t$at, $zero, L%d\n", label_index);
            break;
        case CMP_GE:
            fprintf(fp, "\tslt\t$at, %s, %s\n", mips_register_names[r2], mips_register_names[r1]);
            fprintf(fp, "\tbeq\t$at, $zero, L%d\n", label_index);
            break;
        case CMP_EQ:
            fprintf(fp, "\tbne\t%s, %s, L%d\n", mips_register_names[r1], mips_register_names[r2], label_index);
            break;
        case CMP_NE:
            fprintf(fp, "\tbeq\t%s, %s, L%d\n", mips_register_names[r1], mips_register_names[r2], label_index);
            break;
    }

    return NO_REGISTER;
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

void mips_label(FILE *fp, int label_index){
	fprintf(fp, "L%d:\n", label_index);
}

void mips_jump_to_label(FILE *fp, int label_index){
	fprintf(fp, "\tj\tL%d\n", label_index);
}
