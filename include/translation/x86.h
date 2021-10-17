/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function headers for x86 ASM generators
*/

#ifndef X86_H
#define X86_H

#include <stdio.h>

void x86_preamble(FILE *fp);
void x86_postamble(FILE *fp);

void x86_load_int(FILE *fp, int r, int value);

void x86_print_int(FILE *fp, int r);

int x86_add(FILE *fp, int r1, int r2);
int x86_sub(FILE *fp, int r1, int r2);
int x86_mul(FILE *fp, int r1, int r2);
int x86_div(FILE *fp, int r1, int r2);

void x86_create_global_variable(FILE *fp, char *identifier, int stack_size);
int x86_load_global_variable(FILE *fp, int r, char *identifier, int stack_offset);
int x86_save_global_variable(FILE *fp, int r, char *identifier, int stack_offset);

#endif
