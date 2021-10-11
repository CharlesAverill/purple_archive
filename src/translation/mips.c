#include "translation/mips.h"

char *mips_register_names[] = {"$t0", "$t1", "$t2", "$t3"};

void mips_preamble(FILE *fp){
    fputs(
        ".data"
        "\n"
        "\n"
        ".text\n"
        "main:\n",
    fp);
}

void mips_postamble(FILE *fp){
    fputs(
        "\n"
        "li $v0, 10\n"
        "syscall\n",
    fp);
}

void mips_load(FILE *fp, int r, int value){
    fprintf(fp, "\tli\t%s, %d\n", mips_register_names[r], value);
}

void mips_print_int(FILE *fp, int r){
    fprintf(fp, "\tli\t$v0, 1\n");
    fprintf(fp, "\tadd\t$a0, %s, $zero\n", mips_register_names[r]);
    fprintf(fp, "\tsyscall\n");
}

int mips_add(FILE *fp, int r1, int r2){
    fprintf(fp, "\tadd\t%s, %s, %s\n", mips_register_names[r1], mips_register_names[r1], mips_register_names[r2]);
    return r1;
}

int mips_sub(FILE *fp, int r1, int r2){
    fprintf(fp, "\tsub\t%s, %s, %s\n", mips_register_names[r1], mips_register_names[r1], mips_register_names[r2]);
    return r1;
}

int mips_mul(FILE *fp, int r1, int r2){
    fprintf(fp, "\tmult\t%s, %s\n", mips_register_names[r1], mips_register_names[r2]);
    fprintf(fp, "\tmflo\t%s\n", mips_register_names[r2]);
    return r2;
}

int mips_div(FILE *fp, int r1, int r2){
    fprintf(fp, "\tdiv\t%s, %s\n", mips_register_names[r1], mips_register_names[r2]);
    fprintf(fp, "\tmflo\t%s\n", mips_register_names[r2]);
    return r2;
}
