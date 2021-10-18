/**
 * @file
 * @author CharlesAverill
 * @date   08-Oct-2021
 * @brief Functions for translation to Purple IR
*/

#include "translation/translate.h"

/**
 * Prints the contents of all registers
 */
void print_registers(void)
{
    for (int i = 0; i < n_registers; i++) {
        printf("%d: %d\n", i, free_registers[i]);
    }
}

/**
 * Frees all existing registers by setting their values to 1
*/
void free_all_registers(void)
{
    for (int i = 0; i < n_registers; i++) {
        free_registers[i] = 1;
    }
}

/**
 * Allocates a free register. Exit if there are no remaining registers.
 * @return       The index of the new register
*/
static int allocate_register(void)
{
    for (int i = 0; i < n_registers; i++) {
        if (free_registers[i]) {
            free_registers[i] = 0;
            return i;
        }
    }

    fprintf(stderr, "Ran out of registers\n");
    shutdown(1);
}

/**
 * Add register to the array of free registers
 * @param r_index  Index of the register to add
 */
static void add_free_register(int r_index)
{
    if (free_registers[r_index]) {
        fprintf(stderr, "Error trying to free register %d\n", r_index);
        shutdown(1);
    }

    free_registers[r_index] = 1;
}

/**
 * Initialization code for the translator, including setting the functions of 'generators'
 */
static void initialize_translator(void)
{
    current_asm_mode = args->forced_assembly_mode;

    switch (current_asm_mode) {
    case X86:
        generators.preamble = x86_preamble;
        generators.postamble = x86_postamble;

        generators.load_int = x86_load_int;
        generators.print_int = x86_print_int;

        generators.add = x86_add;
        generators.sub = x86_sub;
        generators.mul = x86_mul;
        generators.div = x86_div;

        generators.compare = x86_compare;

        generators.create_global_variable = x86_create_global_variable;
        generators.load_global_variable = x86_load_global_variable;
        generators.save_global_variable = x86_save_global_variable;
        break;
    case MIPS:
        generators.preamble = mips_preamble;
        generators.postamble = mips_postamble;

        generators.load_int = mips_load_int;
        generators.print_int = mips_print_int;

        generators.add = mips_add;
        generators.sub = mips_sub;
        generators.mul = mips_mul;
        generators.div = mips_div;

        generators.compare = mips_compare;

        generators.create_global_variable = mips_create_global_variable;
        generators.load_global_variable = mips_load_global_variable;
        generators.save_global_variable = mips_save_global_variable;
        break;
    default:
        fprintf(stderr, "Error choosing an assembly mode\n");
        shutdown(1);
    }

    ASM_OUTPUT = fopen(args->filenames[1], "w");
    if (ASM_OUTPUT == NULL) {
        fprintf(stderr, "Error opening %s for writing intermediate assembly\n", args->filenames[1]);
        shutdown(1);
    }
}

/**
 * Exit code for the translator including freeing registers and closing the ASM file
 */
static void exit_translator(void)
{
    fclose(ASM_OUTPUT);

    free_all_registers();
}

/**
 * Loads an integer into a newly-allocated register
 * @param  value               The value to load
 * @return       The index of the register
 */
static int pir_load_int(int value)
{
    int r = allocate_register();

    generators.load_int(ASM_OUTPUT, r, value);

    return r;
}

/**
 * PIR Print Int Logic
 * @param r  Index of the register to print
 */
void pir_print_int(int r)
{
    generators.print_int(ASM_OUTPUT, r);
    add_free_register(r);
}

/**
 * PIR Add Logic
 * @param  left                Index of the left register to add
 * @param  right               Index of the right register to add
 * @return       Index of the register containing the sum
 */
static int pir_add(int left, int right)
{
    int r = generators.add(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Multiply Logic
 * @param  left                Index of the left register to multiply
 * @param  right               Index of the right register to multiply
 * @return       Index of the register containing the product
 */
static int pir_mul(int left, int right)
{
    int r = generators.mul(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Subtract Logic
 * @param  left                Index of the left register to subtract
 * @param  right               Index of the right register to subtract
 * @return       Index of the register containing the difference (left - right)
 */
static int pir_sub(int left, int right)
{
    int r = generators.sub(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Integer Division Logic
 * @param  left                Index of the left register to divide
 * @param  right               Index of the right register to divide
 * @return       Index of the register containing the quotient (left / right)
 */
static int pir_div(int left, int right)
{
    int r = generators.div(ASM_OUTPUT, left, right);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Is Equal logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_equal(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_EQ);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Is Not Equal logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_not_equal(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_NE);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Less Than logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_less_than(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_LT);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Greater Than logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_greater_than(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_GT);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Greater Than or Equal to logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_greater_equal(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_GE);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Less Than or Equal to logic
 * @param  left                Index of the left register to compare
 * @param  right               Index of the right register to compare
 * @return       Index of the register containing the comparison result
 */
static int pir_less_equal(int left, int right)
{
    int r = generators.compare(ASM_OUTPUT, left, right, CMP_LE);
    add_free_register(r == left ? right : left);
    return r;
}

/**
 * PIR Global variable creation logic
 * NOTE: Only generates code for some platforms that support .comm directives
 * @param  identifier               The string defining the name of the variable
 * @param  size                     The size of the data of the variable
 */
void pir_create_global(char *identifier, int size)
{
    generators.create_global_variable(ASM_OUTPUT, identifier, size);
}

/**
 * PIR Load variable from stack logic
 * @param  symbol_index               Symbol table index of the variable to load
 * @return              The register the value has been loaded into
 */
static int pir_load_global(int symbol_index)
{
    int r = allocate_register();
    symbol sym = D_GLOBAL_SYMBOL_TABLE[symbol_index];

    generators.load_global_variable(ASM_OUTPUT, r, sym.name, sym.stack_offset);
    return r;
}

/**
 * PIR Save variable onto stack logic
 * @param  r                          Register containing value to save
 * @param  symbol_index               Symbol table index of symbol to save
 * @return              Register containing value to save
 */
static int pir_save_global(int r, int symbol_index)
{
    symbol sym = D_GLOBAL_SYMBOL_TABLE[symbol_index];
    generators.save_global_variable(ASM_OUTPUT, r, sym.name, sym.stack_offset);
    return r;
}

/**
 * Generates Purple Intermediate Representation (PIR) assembly from a given AST
 * @param  n           The AST Node used to generate PIR
 * @param  r           Index of register used to store variables
 * @return       The value of the AST
*/
int ast_to_pir(AST_Node *n, int r)
{
    int left_register;
    int right_register;
    int out;

    if (n->left) {
        left_register = ast_to_pir(n->left, -1);
    }
    if (n->right) {
        right_register = ast_to_pir(n->right, left_register);
    }

    switch (n->ttype) {
    // Arithmetic
    case T_PLUS:
        out = pir_add(left_register, right_register);
        break;
    case T_MINUS:
        out = pir_sub(left_register, right_register);
        break;
    case T_STAR:
        out = pir_mul(left_register, right_register);
        break;
    case T_SLASH:
        out = pir_div(left_register, right_register);
        break;
    // Comparison
    case T_EQUALS:
        out = pir_equal(left_register, right_register);
        break;
    case T_NOT_EQUALS:
        out = pir_not_equal(left_register, right_register);
        break;
    case T_LESS:
        out = pir_less_than(left_register, right_register);
        break;
    case T_GREATER:
        out = pir_greater_than(left_register, right_register);
        break;
    case T_LESS_EQUAL:
        out = pir_less_equal(left_register, right_register);
        break;
    case T_GREATER_EQUAL:
        out = pir_greater_equal(left_register, right_register);
        break;
    // Literals
    case T_INTLIT:
        out = pir_load_int(n->v.value);
        break;
    // Syntax
    case T_IDENTIFIER:
        out = pir_load_global(n->v.position);
        break;
    case T_LEFT_VALUE_IDENTIFIER:
        out = pir_save_global(r, n->v.position);
    case T_ASSIGNMENT:
        return right_register;
    default:
        fprintf(stderr, "Unknown operator %s\n", token_strings[n->ttype]);
        shutdown(1);
    }

    return out;
}

/**
 * Handle function for generating ASM
 */
void generate_pir(void)
{
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

    printf("%s assembly written to %s\n", asm_mode_names[current_asm_mode], args->filenames[1]);
}
