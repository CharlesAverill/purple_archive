/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function headers for MIPS ASM generators
*/

#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>

#include "data.h"
#include "definitions.h"
#include "symbol_table.h"

void mips_data_section(FILE *fp);

void mips_preamble(FILE *fp);
void mips_postamble(FILE *fp);

void mips_load_int(FILE *fp, int r, int value);

void mips_print_int(FILE *fp, int r);

int mips_add(FILE *fp, int r1, int r2);
int mips_sub(FILE *fp, int r1, int r2);
int mips_mul(FILE *fp, int r1, int r2);
int mips_div(FILE *fp, int r1, int r2);

int mips_compare(FILE *fp, int r1, int r2, Comparison_Mode mode);
int mips_compare_and_jump(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index);

void mips_label(FILE *fp, int label_index);
void mips_jump_to_label(FILE *fp, int label_index);

void mips_create_global_variable(FILE *fp, char *identifier, int stack_size);
int mips_load_global_variable(FILE *fp, int r, char *identifier);
int mips_save_global_variable(FILE *fp, int r, char *identifier);

#endif
