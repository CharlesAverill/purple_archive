/**
 * @file
 * @author CharlesAverill
 * @date   11-Oct-2021
 * @brief Function for parsing statements and compiling them into ASM
*/

#include "parse.h"

// Extern variables
FILE *ASM_OUTPUT;
ASM_Generators generators;

/**
 * Parse print statement
 */
static void print_statement(void)
{
    AST_Node *root;
    int r;

    // First token must be print
    match(T_PRINT);

    // Parse expression, compile it, and print its value
    root = parse_binary_expression(0);
    r = ast_to_pir(root, -1);
    pir_print_int(r);
    free_all_registers();

    // Last token must be a semicolon
    match(T_SEMICOLON);
}

/**
 * Parse variable assignment statement
 */
static void assignment_statement(void)
{
    AST_Node *left;
    AST_Node *right;
    AST_Node *root;

    // First token must be an existing identifier
    match(T_IDENTIFIER);

    // Check position of variable in Global symbol table
    int position = global_symbol_exists(D_IDENTIFIER_BUFFER);
    if (position == -1) {
        print_symbol_table();
        fprintf(stderr, "Undefined variable %s on line %d\n", D_IDENTIFIER_BUFFER, D_LINE_NUMBER);
        shutdown(1);
    }

    // Build AST leaf for left value identifier
    right = make_ast_leaf(T_LEFT_VALUE_IDENTIFIER, position);

    // Match for an equals token
    match(T_EQUALS);

    // Build AST for assignment expression
    left = parse_binary_expression(0);

    // Assembly left and right into AST, generate PIR
    root = make_ast_node(T_EQUALS, left, right, 0);
    ast_to_pir(root, -1);
    free_all_registers();

    // Match for a semicolon
    match(T_SEMICOLON);
}

/**
 * Parse all statements in a file, and compile them into ASM
 */
void parse_statements(void)
{
    while (1) {
        switch (GToken._token) {
        case T_PRINT:
            print_statement();
            break;
        case T_INT:
            variable_declaration();
            break;
        case T_IDENTIFIER:
            assignment_statement();
            break;
        case T_EOF:
            return;
        default:
            fprintf(stderr, "Syntax error on line %d\n", D_LINE_NUMBER);
        }
    }
}
