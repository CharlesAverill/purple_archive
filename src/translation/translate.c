/**
 * @file
 * @author CharlesAverill
 * @date   08-Oct-2021
 * @brief Functions for translation to Purple IR
*/

#include "translation/translate.h"
    
void print_registers(void){
    for(int i = 0; i < n_registers; i++){
        printf("%d: %d\n", i, free_registers[i]);
    }
}

/**
 * Frees all existing registers by setting their values to 1
*/
void free_all_registers(void){
    for(int i = 0; i < n_registers; i++){
        free_registers[i] = 1;
    }
}

/**
 * Allocates a free register. Exit if there are no remaining registers.
 * @return       The index of the new register
*/
static int allocate_register(void){
    for(int i = 0; i < n_registers; i++){
        if(free_registers[i]){
            free_registers[i] = 0;
            return i;
        }
    }
    
    fprintf(stderr, "Ran out of registers\n");
    exit(1);
}

/**
 * Add register to the array of free registers
*/
static void add_free_register(int r_index){
    if(free_registers[r_index]){
        fprintf(stderr, "Error trying to free register %d\n", r_index);
        exit(1);
    }
    
    free_registers[r_index] = 1;
}

static void initialize_translator(void){
    current_asm_mode = MIPS;
    
    switch(current_asm_mode){
    case X86:
        generators.preamble = x86_preamble;
        generators.postamble = x86_postamble;
            
        generators.load = x86_load;
        generators.print_int = x86_print_int;
        
        generators.add = x86_add;
        generators.sub = x86_sub;
        generators.mul = x86_mul;
        generators.div = x86_div;
        break;
    case MIPS:
        generators.preamble = mips_preamble;
        generators.postamble = mips_postamble;
            
        generators.load = mips_load;
        generators.print_int = mips_print_int;
        
        generators.add = mips_add;
        generators.sub = mips_sub;
        generators.mul = mips_mul;
        generators.div = mips_div;
        break;
    default:
        fprintf(stderr, "Error choosing an assembly mode\n");
        exit(1);
    }
    
    ASM_OUTPUT = fopen("a.asm", "w");
    if(ASM_OUTPUT == NULL){
        fprintf(stderr, "Error opening %s for writing intermediate assembly\n", "a.asm");
        exit(1);
    }
}

static void exit_translator(void){
    fclose(ASM_OUTPUT);
    
    free_all_registers();
}

/**
 * Loads a value into a newly-allocated register
*/
static int pir_load(int value){
    int r = allocate_register();
    
    generators.load(ASM_OUTPUT, r, value);
    
    return r;
}

/**
 * PIR Print Int logic
*/
void pir_print_int(int r){
    generators.print_int(ASM_OUTPUT, r);
    add_free_register(r);
}

/**
 * PIR Add logic
*/
static int pir_add(int left, int right){
    int r = generators.add(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Multiply Logic
*/
static int pir_mul(int left, int right){
    int r = generators.mul(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Subtract Logic
*/
static int pir_sub(int left, int right){
    int r = generators.sub(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Divide Logic
*/
static int pir_div(int left, int right){
    int r = generators.div(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * Generates Purple Intermediate Representation (PIR) assembly from a given AST
 * @param  n           The AST Node used to generate PIR
 * @return       The value of the AST
*/
int ast_to_pir(AST_Node *n){
    int left_register;
    int right_register;
    
    if(n->left){
        left_register = ast_to_pir(n->left);
    }
    if(n->right){
        right_register = ast_to_pir(n->right);
    }
    
    switch(n->ttype){
        case T_PLUS:
            return pir_add(left_register, right_register);
        case T_MINUS:
            return pir_sub(left_register, right_register);
        case T_STAR:
            return pir_mul(left_register, right_register);
        case T_SLASH:
            return pir_div(left_register, right_register);
        case T_INTLIT:
            return pir_load(n->value);
        default:
            fprintf(stderr, "Unknown operator %d\n", n->ttype);
            exit(1);
    }
}

int generate_pir(void){
    initialize_translator();
    
    // Reset all registers
    free_all_registers();
    
    // Add the assembly preamble
    generators.preamble(ASM_OUTPUT);
    
    // Initialize the Scanner
    scan(&GToken);
    
    // Calculate and print the value of the AST
    parse_statements();
    
    // Add the assembly postamble
    generators.postamble(ASM_OUTPUT);
    
    exit_translator();
    
    printf("%s assembly written to %s\n", asm_mode_names[current_asm_mode], "a.asm");
}