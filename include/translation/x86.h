/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function headers for x86 ASM generators
*/

#ifndef X86_H
#define X86_H

#include <stdio.h>

#include "data.h"
#include "definitions.h"
#include "symbol_table.h"

void x86_data_section(FILE *fp);

void x86_preamble(FILE *fp);
void x86_postamble(FILE *fp);

void x86_load_int(FILE *fp, int r, int value);

void x86_print_int(FILE *fp, int r);

int x86_add(FILE *fp, int r1, int r2);
int x86_sub(FILE *fp, int r1, int r2);
int x86_mul(FILE *fp, int r1, int r2);
int x86_div(FILE *fp, int r1, int r2);

int x86_compare(FILE *fp, int r1, int r2, Comparison_Mode mode);
int x86_compare_and_jump(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index);

void x86_label(FILE *fp, int label_index);
void x86_jump_to_label(FILE *fp, int label_index);

void x86_enter_scope(FILE *fp, symbol_table *symtab);
void x86_leave_scope(FILE *fp);

void x86_load_variable(FILE *fp, int r, char *identifier);
void x86_save_variable(FILE *fp, int r, char *identifier);

#endif
