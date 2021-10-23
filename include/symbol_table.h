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

void init_global_symbol_table(void);
symbol_table *make_symbol_table(symbol_table *parent);
int symbol_exists(symbol_table *symtab, char *name);
int insert_symbol(symbol_table *symtab, char *name, Token_Type datatype);

symbol *get_symbol(symbol_table *stack_top, char *name);

void print_symbol_table(symbol_table *symtab);

#endif
