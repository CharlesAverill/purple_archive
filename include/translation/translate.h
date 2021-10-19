/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function headers and definitions for translation of PIR into ASM
*/

#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "definitions.h"
#include "parse.h"
#include "tree.h"

#include "translation/mips.h"
#include "translation/x86.h"

#define n_registers 4

/**The file pointer for the output assembly file*/
extern FILE *ASM_OUTPUT;

/**The array defining free registers. If free_registers[r1] == 1, r1 is free*/
static int free_registers[n_registers];

/**Enum defining supported assembly modes*/
typedef enum Assembly_Mode {
    X86,
    MIPS,
} Assembly_Mode;

#define N_SUPPORTED_ASM_MODES 2
/**Names of supported assembly modes*/
static char *asm_mode_names[N_SUPPORTED_ASM_MODES] = {"x86", "MIPS"};

/**Assembly mode to use during compilation*/
static Assembly_Mode current_asm_mode = -1;

/**
 * @struct ASM_Generators
 * @brief Struct containing pointers to the asm-generating functions used by the translator
 */
typedef struct ASM_Generators {
    /**ASM preamble code*/
    void (*preamble)(FILE *fp);
    /**ASM postamble code*/
    void (*postamble)(FILE *fp);

    /**Load an integer value into register r*/
    void (*load_int)(FILE *fp, int r, int value);
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

    /**Compare the values of two registers and place it in a register*/
    int (*compare)(FILE *fp, int r1, int r2, Comparison_Mode mode);
	/**Compare the values of two registers and jump to a label*/
	int (*compare_and_jump)(FILE *fp, int r1, int r2, Comparison_Mode mode, int label_index);
	
	/**Generate a label*/
	void (*label)(FILE *fp, int label_index);
	/**Jump to a label*/
	void (*jump_to_label)(FILE *fp, int label_index);

    /**Create a global variable*/
    void (*create_global_variable)(FILE *fp, char *identifier, int stack_size);
    /**Load a variable from the stack into a register*/
    int (*load_global_variable)(FILE *fp, int r, char *identifier, int stack_offset);
    /**Save a variable from a register onto the stack*/
    int (*save_global_variable)(FILE *fp, int r, char *identifier, int stack_offset);
} ASM_Generators;

/**Struct containing the ASM generator functions for the current compilation*/
extern ASM_Generators generators;

void free_all_registers(void);
void pir_print_int(int r);

void pir_create_global(char *identifier, int size);

int ast_to_pir(AST_Node *n, int r, Token_Type previous_operation);
void generate_pir(void);

#endif
