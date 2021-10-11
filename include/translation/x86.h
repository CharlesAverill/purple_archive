#ifndef X86_H
#define X86_H

#include <stdio.h>

void x86_preamble(FILE *fp);
void x86_postamble(FILE *fp);

void x86_load(FILE *fp, int r, int value);

void x86_print_int(FILE *fp, int r);

int x86_add(FILE *fp, int r1, int r2);
int x86_sub(FILE *fp, int r1, int r2);
int x86_mul(FILE *fp, int r1, int r2);
int x86_div(FILE *fp, int r1, int r2);

#endif