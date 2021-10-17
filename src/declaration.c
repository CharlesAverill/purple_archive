/**
 * @file
 * @author CharlesAverill
 * @date   17-Oct-2021
 * @brief Logic for variable declarations
*/

#include "declaration.h"

/**
 * Parse a variable declaration statement
 */
void variable_declaration(void)
{
    // Ensure we have a type definition and identifier definition
    match(T_INT);
    match(T_IDENTIFIER);

    // Add symbol name to table
    int position = insert_global_symbol(D_IDENTIFIER_BUFFER);
    pir_create_global(D_IDENTIFIER_BUFFER, D_GLOBAL_SYMBOL_TABLE[position].size);

    // Ensure semicolon
    match(T_SEMICOLON);
}
