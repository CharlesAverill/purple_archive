/**
 * @file
 * @author CharlesAverill
 * @date   16-Oct-2021
 * @brief Function headers and definitions for Symbol Tables
*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "data.h"
#include "definitions.h"

void init_symbol_table(void);
int global_symbol_exists(char *name);
int insert_global_symbol(char *name);

void print_symbol_table(void);

#endif