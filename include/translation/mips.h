#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>

void mips_preamble(FILE *fp);
void mips_postamble(FILE *fp);

void mips_load(FILE *fp, int r, int value);

void mips_print_int(FILE *fp, int r);

int mips_add(FILE *fp, int r1, int r2);
int mips_sub(FILE *fp, int r1, int r2);
int mips_mul(FILE *fp, int r1, int r2);
int mips_div(FILE *fp, int r1, int r2);

#endif