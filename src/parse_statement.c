/**
 * @file
 * @author charlesaverill
 * @date   11-Oct-2021
 * @brief Function for parsing statements and compiling them into ASM
*/

#include "parse.h"

// Extern variables
token GToken;
FILE *ASM_OUTPUT;
ASM_Generators generators;

/**
 * Forces an exit if GToken's Token_Type does not match the provided Token_Type
 * @param ttype  The desired Token_Type to match
 * @param str    The name of the keyword to match
 */
static void match(Token_Type ttype, char *str)
{
    if (GToken._token == ttype) {
        scan(&GToken);
    } else {
        fprintf(stderr, "%s expected on line %d but got %s instead\n", str, D_LINE_NUMBER,
                token_strings[ttype]);
        exit(1);
    }
}

/**
 * Parse all statements in a file, and compile them into ASM
 */
void parse_statements(void)
{
    AST_Node *AST_root;
    int r;

    while (1) {
        // First token must be a print statement
        match(T_PRINT, "print");

        // Parse expression, compile it, and print its value
        AST_root = parse_binary_expression(0);
        r = ast_to_pir(AST_root);
        pir_print_int(r);

        // Check for semicolon and EOF
        match(T_SEMICOLON, ";");
        if (GToken._token == T_EOF) {
            break;
        }
    }
}
