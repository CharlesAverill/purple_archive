#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "definitions.h"
#include "parse.h"
#include "tree.h"

#include "translation/x86.h"
#include "translation/mips.h"

#define n_registers 4

/**The file pointer for the output assembly file*/
FILE *ASM_OUTPUT;

/**The array defining free registers. If free_registers[r1] == 1, r1 is free*/
static int free_registers[n_registers];

/**Enum defining supported assembly modes*/
typedef enum Assembly_Mode {
    X86,
    MIPS,
} Assembly_Mode;

/**Names of supported assembly modes*/
static char *asm_mode_names[] = {"x86", "MIPS"};

/**Assembly mode to use during compilation*/
static Assembly_Mode current_asm_mode = -1;

/**Struct containing pointers to the asm-generating functions used by the translator*/
typedef struct ASM_Generators{
    /**ASM preamble code*/
    void (*preamble)(FILE *fp);
    /**ASM postamble code*/
    void (*postamble)(FILE *fp);

    /**Load an integer value into register r*/
    void (*load)(FILE *fp, int r, int value);

    /**Print an integer in register r*/
    void (*print_int)(FILE *fp, int r);

    /**Add two integers*/
    int (*add)(FILE *fp, int r1, int r2);
    /**Subtract two integers*/
    int (*sub)(FILE *fp, int r1, int r2);
    /**Multiply two integers*/
    int (*mul)(FILE *fp, int r1, int r2);
    /**Divide two integers*/
    int (*div)(FILE *fp, int r1, int r2);
} ASM_Generators;

ASM_Generators generators;

void free_all_registers(void);
void pir_print_int(int r);

int ast_to_pir(AST_Node *n);
int generate_pir(void);

#endif
