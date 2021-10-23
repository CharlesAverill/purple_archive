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
        generators.data_section = x86_data_section;

        generators.preamble = x86_preamble;
        generators.postamble = x86_postamble;

        generators.load_int = x86_load_int;
        generators.print_int = x86_print_int;

        generators.add = x86_add;
        generators.sub = x86_sub;
        generators.mul = x86_mul;
        generators.div = x86_div;

        generators.compare = x86_compare;
        generators.compare_and_jump = x86_compare_and_jump;
        generators.label = x86_label;
        generators.jump_to_label = x86_jump_to_label;

        generators.enter_scope = x86_enter_scope;
        generators.leave_scope = x86_leave_scope;

        generators.load_variable = x86_load_variable;
        generators.save_variable = x86_save_variable;
        break;
    case MIPS:
        generators.data_section = mips_data_section;

        generators.preamble = mips_preamble;
        generators.postamble = mips_postamble;

        generators.load_int = mips_load_int;
        generators.print_int = mips_print_int;

        generators.add = mips_add;
        generators.sub = mips_sub;
        generators.mul = mips_mul;
        generators.div = mips_div;

        generators.compare = mips_compare;
        generators.compare_and_jump = mips_compare_and_jump;
        generators.label = mips_label;
        generators.jump_to_label = mips_jump_to_label;

        generators.enter_scope = mips_enter_scope;
        generators.leave_scope = mips_leave_scope;

        generators.load_variable = mips_load_variable;
        generators.save_variable = mips_save_variable;
        break;
    default:
        fprintf(stderr, "Error choosing an assembly mode\n");
        shutdown(1);
    }

    ASM_OUTPUT = fopen(args->filenames[1], "w");
    if (ASM_OUTPUT == NULL) {
        fprintf(stderr, "Error opening %s for writing assembly\n", args->filenames[1]);
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
 * PIR Enter Scope Logic
 * @param symtab    Symbol table of the new scope
 */

static void pir_enter_scope(symbol_table *symtab) { generators.enter_scope(ASM_OUTPUT, symtab); }

/**
 * PIR Leave Scope Logic
 */
static void pir_leave_scope() { generators.leave_scope(ASM_OUTPUT); }

/**
 * PIR Load variable logic
 * @param  identifier
 * @return              The register the value has been loaded into
 */
static int pir_load(char *identifier)
{
    int r = allocate_register();
    generators.load_variable(ASM_OUTPUT, r, identifier);
    return r;
}

/**
 * PIR Save variable logic
 * @param  r                          Register containing value to save
 * @param  symbol_index               Symbol table index of symbol to save
 * @return              Register containing value to save
 */
static int pir_save(int r, char *identifier)
{
    generators.save_variable(ASM_OUTPUT, r, identifier);
    return r;
}

// Index of branch labels to be generated
static int LABEL_INDEX = 0;

/**
 * Generates Purple Intermediate Representation (PIR) assembly from a given if-statement AST
 * @param  n                         The AST Node used to generate PIR
 * @return       NO_REGISTER
*/
static int if_ast_to_pir(AST_Node *n)
{
    int false_label_index;
    int exit_label_index;

    // Generate label index for the false branch
    false_label_index = LABEL_INDEX++;

    // Generate label index for exiting the if statement
    // If there is no nelse, false_label_index = exit_label_index
    if (n->right) {
        exit_label_index = LABEL_INDEX++;
    }

    // Generate conditional code and a jump to the false label
    ast_to_pir(n->left, false_label_index, n->ttype);
    free_all_registers();

    // Generate ASM for compound statement if true
    ast_to_pir(n->mid, NO_REGISTER, n->ttype);
    free_all_registers();

    // If an else exists, generate the jump for it
    if (n->right) {
        generators.jump_to_label(ASM_OUTPUT, exit_label_index);
    }

    // Add the false label
    generators.label(ASM_OUTPUT, false_label_index);

    // If an else exists, generate the else ASM and the label to skip it
    if (n->right) {
        ast_to_pir(n->right, NO_REGISTER, n->ttype);
        free_all_registers();

        generators.label(ASM_OUTPUT, exit_label_index);
    }

    return NO_REGISTER;
}

/**
 * Generates Purple Intermediate Representation (PIR) assembly from a given while-statement AST
 * @param  n                         The AST Node used to generate PIR
 * @return       NO_REGISTER
*/
static int while_ast_to_pir(AST_Node *n)
{
    int loop_label_index;
    int exit_label_index;

    // Generate label index for the loop and exit branches
    loop_label_index = LABEL_INDEX++;
    exit_label_index = LABEL_INDEX++;

    generators.label(ASM_OUTPUT, loop_label_index);

    // Generate conditional code and a jump to the exit label
    ast_to_pir(n->left, exit_label_index, n->ttype);
    free_all_registers();

    // Generate ASM for compound statement
    ast_to_pir(n->right, NO_REGISTER, n->ttype);
    free_all_registers();

    // Jump back to the loop label
    generators.jump_to_label(ASM_OUTPUT, loop_label_index);

    // Add the exit label
    generators.label(ASM_OUTPUT, exit_label_index);

    return NO_REGISTER;
}

/**
 * Generates Purple Intermediate Representation (PIR) assembly from a given AST
 * @param  n                         The AST Node used to generate PIR
 * @param  r                         Index of register used to store variables
 * @param  previous_operation        Operation of the previously-analyzed node's parent
 * @return       The register containing the value of the AST
*/
int ast_to_pir(AST_Node *n, int r, Token_Type previous_operation)
{
    int left_register;
    int right_register;
    int out;

    // If-statement AST parsing takes precedence

    switch (n->ttype) {
    case T_IF:
        return if_ast_to_pir(n);
    case T_WHILE:
        return while_ast_to_pir(n);
    case T_SCOPE:
        pir_enter_scope(n->v.scope_symbol_table);
        ast_to_pir(n->left, NO_REGISTER, n->ttype);
        pir_leave_scope();
        return NO_REGISTER;
    case T_AST_GLUE:

        // Generate left side
        ast_to_pir(n->left, NO_REGISTER, n->ttype);
        free_all_registers();

        // Generate right side
        ast_to_pir(n->right, NO_REGISTER, n->ttype);
        free_all_registers();

        return NO_REGISTER;
    }

    // Perform general AST parsing if not an IF statement or Glue

    if (n->left) {
        left_register = ast_to_pir(n->left, -1, n->ttype);
    }
    if (n->right) {
        right_register = ast_to_pir(n->right, left_register, n->ttype);
    }

    Comparison_Mode cmp_mode;

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
    case T_NOT_EQUALS:
    case T_LESS:
    case T_GREATER:
    case T_LESS_EQUAL:
    case T_GREATER_EQUAL:

        switch (n->ttype) {
        case T_EQUALS:
            cmp_mode = CMP_EQ;
            break;
        case T_NOT_EQUALS:
            cmp_mode = CMP_NE;
            break;
        case T_LESS:
            cmp_mode = CMP_LT;
            break;
        case T_GREATER:
            cmp_mode = CMP_GT;
            break;
        case T_LESS_EQUAL:
            cmp_mode = CMP_LE;
            break;
        case T_GREATER_EQUAL:
            cmp_mode = CMP_GE;
            break;
        }

        // For now, make if comparisons generate jumps, and general comparisons fill a register with 1 or 0
        if (previous_operation == T_IF || previous_operation == T_WHILE) {
            int val =
                generators.compare_and_jump(ASM_OUTPUT, left_register, right_register, cmp_mode, r);
            free_all_registers();
            return val;
        } else {
            out = generators.compare(ASM_OUTPUT, left_register, right_register, cmp_mode);
            add_free_register(out == left_register ? right_register : left_register);
        }
        break;
    // Literals
    case T_INTLIT:
        out = pir_load_int(n->v.value);
        break;
        // Syntax
    case T_PRINT:
        pir_print_int(left_register);
        free_all_registers();
        out = NO_REGISTER;
        break;
    case T_IDENTIFIER:
        out = pir_load(n->v.identifier);
        break;
    case T_ASSIGNMENT:
        out = right_register;
        break;
        // AST-specific
    case T_AST_LEFT_VALUE_IDENTIFIER:
        out = pir_save(r, n->v.identifier);
        break;
    default:
        fprintf(stderr, "Unknown operator during ASM generation - %s\n", token_strings[n->ttype]);
        shutdown(1);
    }

    return out;
}

/**
 * Handle function for generating ASM
 */
void generate_pir(AST_Node *root)
{
    initialize_translator();

    generators.data_section(ASM_OUTPUT);

    // Reset all registers
    free_all_registers();

    // Add the assembly preamble
    generators.preamble(ASM_OUTPUT);

    // Parse everything into a single AST
    ast_to_pir(root, NO_REGISTER, 0);

    // Add the assembly postamble
    generators.postamble(ASM_OUTPUT);

    exit_translator();

    printf("%s assembly written to %s\n", asm_mode_names[current_asm_mode], args->filenames[1]);
}
