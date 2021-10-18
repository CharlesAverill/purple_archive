/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function headers for MIPS ASM generators
*/

#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>

#include "definitions.h"

void mips_preamble(FILE *fp);
void mips_postamble(FILE *fp);

void mips_load_int(FILE *fp, int r, int value);

void mips_print_int(FILE *fp, int r);

int mips_add(FILE *fp, int r1, int r2);
int mips_sub(FILE *fp, int r1, int r2);
int mips_mul(FILE *fp, int r1, int r2);
int mips_div(FILE *fp, int r1, int r2);

int mips_compare(FILE *fp, int r1, int r2, Comparison_Mode mode);

void mips_create_global_variable(FILE *fp, char *identifier, int stack_size);
int mips_load_global_variable(FILE *fp, int r, char *identifier, int stack_offset);
int mips_save_global_variable(FILE *fp, int r, char *identifier, int stack_offset);

#endif
